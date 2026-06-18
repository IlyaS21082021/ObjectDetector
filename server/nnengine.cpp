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
  input_tensor_values_.resize(input_width_ * input_height_ * 3);
  channel_size_ = input_width_ * input_height_;
}

bool NNEngine::ProcessImage(const cv::Mat& image) {
  if (image.empty()) {
    std::cerr << "Cannot load image\n";
    return false;
  }
  res_boxes_.clear();

  //calc scale coeffs
  cv::Mat resized;
  cv::resize(image, resized, cv::Size(input_width_, input_height_));
  sx_ = static_cast<float>(image.cols) / input_width_;
  sy_ = static_cast<float>(image.rows) / input_height_;

  cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);
  resized.convertTo(resized, CV_32F, 1.0 / 255.0);

  //fill tensors
  for (int y = 0; y < input_height_; ++y) {
    for (int x = 0; x < input_width_; ++x) {
      cv::Vec3f pixel = resized.at<cv::Vec3f>(y, x);
      input_tensor_values_[0 * channel_size_ + y * input_width_ + x] = pixel[0];
      input_tensor_values_[1 * channel_size_ + y * input_width_ + x] = pixel[1];
      input_tensor_values_[2 * channel_size_ + y * input_width_ + x] = pixel[2];
    }
  }

  const char* input_names[] = {input_name_str_.c_str()};
  const char* output_names[] = {output_name_str_.c_str()};

  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
      memory_info_, input_tensor_values_.data(), input_tensor_values_.size(),
      input_shape_.data(), input_shape_.size());

  //run object detection
  output_tensors_ = session_->Run(Ort::RunOptions{nullptr}, input_names,
                                  &input_tensor, 1, output_names, 1);

  auto type_info = output_tensors_[0].GetTensorTypeAndShapeInfo();
  objects_count_ = type_info.GetShape()[2];
  return true;
}

const float* NNEngine::GetXP() {
  return output_tensors_[0].GetTensorData<float>();
}

const float* NNEngine::GetYP() {
  return output_tensors_[0].GetTensorData<float>() + objects_count_;
}

const float* NNEngine::GetWidthP() {
  return output_tensors_[0].GetTensorData<float>() + objects_count_ * 2;
}

const float* NNEngine::GetHeightP() {
  return output_tensors_[0].GetTensorData<float>() + objects_count_ * 3;
}

const float* NNEngine::GetObjTypeValuesP(int obj_type) {
  return output_tensors_[0].GetTensorData<float>() +
         objects_count_ * (obj_type + 4);
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

const std::vector<VBox>& NNEngine::GetBoxes(const std::set<int>& obj_types,
                                             float min_obj_type_score) {
  if (!res_boxes_.empty()) return res_boxes_;

  const float* px = GetXP();
  const float* py = GetYP();
  const float* pw = GetWidthP();
  const float* ph = GetHeightP();

  std::vector<std::vector<VBox>> output_boxes;
  for (int obj_type : obj_types) {
    //get class probability of the object
    const float* pc = GetObjTypeValuesP(obj_type);
    bool vector_added = false;
    for (size_t i = 0; i < objects_count_; ++i) {
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
  res_boxes_ = DelDoubleObjects(output_boxes);
  for (auto& b : res_boxes_) b.ScaleData(sx_, sy_);
  return res_boxes_;
}
