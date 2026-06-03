#include "aux.h"

#include <stdexcept>

void CheckInputData(const YAML::Node& node) {
  if (!node["vertices"]) {
    throw std::runtime_error("Vertices block is empty");
  }
  
  if (!node["obj_classes"]) {
    throw std::runtime_error("Obj_classes is empty");
  }

  if (!node["type"]) {
    throw std::runtime_error("Polygon type is not set");
  }

  if (!node["threshold"]) {
    throw std::runtime_error("Polygon threshold is not set");
  }

  if (!node["priority"]) {
    throw std::runtime_error("Polygon priority is not set");  
  }
}

std::vector<Polygon> LoadPolygons(const YAML::Node& config) {
  if (!config["polygons"])
    throw std::runtime_error("No polygons in file");

  std::vector<Polygon> res;
  for (const auto& node : config["polygons"]) {
    CheckInputData(node);
    std::vector<float> poly_vertices;
    for (const auto& vertex : node["vertices"]) {
      poly_vertices.push_back(vertex["x"].as<int>());
      poly_vertices.push_back(vertex["y"].as<int>());
    }
    if (poly_vertices.size() < 8) {
      throw std::runtime_error("Wrong number of vertices (should be > 3)");
    }

    std::set<int> obj_classes;
    for (const auto& cl : node["obj_classes"]) {
      obj_classes.insert(cl.as<int>());
    }

    PolyType poly_type_ = (node["type"].as<std::string>() == "include")
                              ? PolyType::kInclude
                              : PolyType::kExclude;
    res.emplace_back(poly_vertices, obj_classes, poly_type_, node["threshold"].as<float>(),
                     node["priority"].as<int>());
  }
  return res;
}

std::vector<uchar> LoadImage(const std::string& img_path) {
  cv::Mat image = cv::imread(img_path);
  if (image.empty()) {
    throw std::runtime_error("Cannot load image");
  }
  std::vector<uchar> encoded_image;
  if (!cv::imencode(".jpg", image, encoded_image)) {
    throw std::runtime_error("Cannot encode image");
  }
  return encoded_image;
}

void LoadPolygonsToReq(ObjectFinder::ImageRequest& req,
                       const std::vector<Polygon>& polygons) {
  for (const auto& poly : polygons) {
    auto* proto_poly = req.add_polygons();
    for (const Point& p : poly.GetPoints()) {
      proto_poly->add_coords(p.x);
      proto_poly->add_coords(p.y);
    }
    for (int c : poly.obj_classes) {
      proto_poly->add_objclasses(c);
    }
    proto_poly->set_threshold(poly.threshold);
    proto_poly->set_priority(poly.priority);
    proto_poly->set_type(poly.poly_type == PolyType::kInclude
                             ? ObjectFinder::INCLUDE
                             : ObjectFinder::EXCLUDE);
  }
}
