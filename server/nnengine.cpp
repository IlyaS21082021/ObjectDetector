#include "nnengine.h"

#include <stdexcept>

NNEngine::NNEngine(const std::string& model_path, int width, int height)
    : env_(ORT_LOGGING_LEVEL_WARNING, "yolo"),
      input_width_(width),
      input_height_(height),
      memory_info_(
          Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {
  session_options_.SetGraphOptimizationLevel(
      GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
  session_ = std::make_unique<Ort::Session>(
      env_, model_path.c_str(), session_options_);

  Ort::AllocatorWithDefaultOptions allocator;
  input_name_str_ = session_->GetInputNameAllocated(0, allocator).get();
  output_name_str_ = session_->GetOutputNameAllocated(0, allocator).get();

  input_shape_ = {1, 3, input_width_, input_height_};
  channel_size_ = input_width_ * input_height_;
}

bool NNEngine::ProcessImage(const cv::Mat& image, EngineContextThread& ctx) {
  if (image.empty()) {
    std::cerr << "Cannot load image\n";
    return false;
  }

  //calc scale coeffs
  cv::Mat resized;
  cv::resize(image, resized, cv::Size(input_width_, input_height_));  

  cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);
  resized.convertTo(resized, CV_32F, 1.0 / 255.0);

  //fill tensors
  for (int y = 0; y < input_height_; ++y) {
    for (int x = 0; x < input_width_; ++x) {
      cv::Vec3f pixel = resized.at<cv::Vec3f>(y, x);
      ctx.input_tensor_values[0 * channel_size_ + y * input_width_ + x] = pixel[0];
      ctx.input_tensor_values[1 * channel_size_ + y * input_width_ + x] = pixel[1];
      ctx.input_tensor_values[2 * channel_size_ + y * input_width_ + x] = pixel[2];
    }
  }

  const char* input_names[] = {input_name_str_.c_str()};
  const char* output_names[] = {output_name_str_.c_str()};

  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
      memory_info_, ctx.input_tensor_values.data(), ctx.input_tensor_values.size(),
      input_shape_.data(), input_shape_.size());

  //run object detection
  ctx.output_tensors = session_->Run(Ort::RunOptions{nullptr}, input_names,
                                  &input_tensor, 1, output_names, 1);

  auto type_info = ctx.output_tensors[0].GetTensorTypeAndShapeInfo();
  ctx.objects_count = type_info.GetShape()[2];
  return true;
}

const float* NNEngine::GetXP(const EngineContextThread& ctx) const {
  return ctx.output_tensors[0].GetTensorData<float>();
}

const float* NNEngine::GetYP(const EngineContextThread& ctx) const {
  return ctx.output_tensors[0].GetTensorData<float>() + ctx.objects_count;
}

const float* NNEngine::GetWidthP(const EngineContextThread& ctx) const {
  return ctx.output_tensors[0].GetTensorData<float>() + ctx.objects_count * 2;
}

const float* NNEngine::GetHeightP(const EngineContextThread& ctx) const {
  return ctx.output_tensors[0].GetTensorData<float>() + ctx.objects_count * 3;
}

const float* NNEngine::GetObjTypeValuesP(int obj_type, 
                                        const EngineContextThread& ctx) const {
  return ctx.output_tensors[0].GetTensorData<float>() +
         ctx.objects_count * (obj_type + 4);
}

std::vector<VBox> NNEngine::DelDoubleObjects(
    std::vector<std::vector<VBox>>& data) {
  std::vector<VBox> res;
  const float kDif = 5.0f;

  //cycle over groups
  for (const auto& one_type_objs : data) {
    size_t start_group_idx = res.size();
    res.push_back(one_type_objs[0]); //put first obj of group

    //cycle over objects of one group
    for (size_t i = 1; i < one_type_objs.size(); ++i) {
      bool same = false;
      //compare others objects of group with unique in res
      for (size_t j = start_group_idx; j < res.size(); ++j) {
        float dx1 = std::abs(one_type_objs[i].x1 - res[j].x1);
        float dy1 = std::abs(one_type_objs[i].y1 - res[j].y1);
        float dx2 = std::abs(one_type_objs[i].x2 - res[j].x2);
        float dy2 = std::abs(one_type_objs[i].y2 - res[j].y2);

        if (dx1 <= kDif && dy1 <= kDif && dx2 <= kDif && 
            dy2 <= kDif) {
          same = true;
          break;
        }
      }
      if (!same) res.push_back(one_type_objs[i]);
    }
  }
  return res;
}

const std::vector<VBox> NNEngine::GetBoxes(const std::set<int>& obj_types,
                                             float min_obj_type_score,
                                             EngineContextThread& ctx) const {
  if (!ctx.res_boxes.empty()) return ctx.res_boxes;

  const float* px = GetXP(ctx);
  const float* py = GetYP(ctx);
  const float* pw = GetWidthP(ctx);
  const float* ph = GetHeightP(ctx);

  std::vector<std::vector<VBox>> output_boxes;
  for (int obj_type : obj_types) {
    //get class probability of the object
    const float* pc = GetObjTypeValuesP(obj_type, ctx);
    bool vector_added = false;
    for (size_t i = 0; i < ctx.objects_count; ++i) {
      if (pc[i] > min_obj_type_score) {
        if (!vector_added) {
          output_boxes.emplace_back(); //create new group
          vector_added = true;
        }
        //add obj to the group
        output_boxes.back().emplace_back(px[i], py[i], pw[i], ph[i], obj_type);
      }
    }
  }

  //output_boxes - objects sorted by group
  ctx.res_boxes = DelDoubleObjects(output_boxes);
  for (auto& b : ctx.res_boxes) b.ScaleData(ctx.sx, ctx.sy);
  return ctx.res_boxes;
}


EngineContextThread NNEngine::SetContextForThread(const cv::Mat& img) const {
  EngineContextThread ctx;
  ctx.sx = static_cast<float>(img.cols) / input_width_;
  ctx.sy = static_cast<float>(img.rows) / input_height_;
  ctx.input_tensor_values.resize(input_width_ * input_height_ * 3); //todo: optimize - cyclic memory buffer with atomic index
  return ctx;
}