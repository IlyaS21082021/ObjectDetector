#include "service.h"

#include <set>
#include <vector>

#include "poly.h"
#include "utils.h"

ObjectFinderService::ObjectFinderService()
    : nn_engine_("yolo11n.onnx", 640, 640) {}

grpc::Status ObjectFinderService::SelectObjects(
    grpc::ServerContext* /*context*/,
    const ObjectFinder::ImageRequest* request,
    ObjectFinder::ImageResponse* response) {
  const std::string& img_data = request->image();
  cv::Mat image = cv::imdecode(
      std::vector<uchar>(img_data.begin(), img_data.end()), cv::IMREAD_COLOR);
  if (image.empty()) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid image");
  }

  if (!nn_engine_.ProcessImage(image)) {
    return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to process image");
  }

  std::set <int> g_obj_classes;
  //read polygons
  std::vector<Polygon> polygons;
  for (const auto& poly : request->polygons()) {
    std::vector<float> points(poly.coords().begin(), poly.coords().end());
    std::set<int> obj_classes(poly.objclasses().begin(), 
                                 poly.objclasses().end());
    PolyType poly_type = (poly.type() == ObjectFinder::EXCLUDE) ? 
                                       PolyType::kExclude : PolyType::kInclude;
    polygons.emplace_back(points, obj_classes, poly_type, poly.threshold(), 
                          poly.priority());
    for (auto cl : obj_classes) {
      g_obj_classes.insert(cl);
    }
  }
  //del double boxes
  auto unique_boxes = nn_engine_.GetBoxes(g_obj_classes, 0.55f);  
  //obtain only needed boxes
  auto filtered_boxes = BoxFilter(unique_boxes, polygons);
  for (const auto& b : filtered_boxes) {
    cv::rectangle(image, cv::Rect(b.x1, b.y1, b.x2 - b.x1, b.y2 - b.y1),
                  cv::Scalar(0, 255, 0), 2);
  }

  std::vector<uchar> encoded;
  if (!cv::imencode(".jpg", image, encoded)) {
    return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to encode image");
  }

  response->set_image(reinterpret_cast<const char*>(encoded.data()),
                      encoded.size());
  return grpc::Status::OK;
}
