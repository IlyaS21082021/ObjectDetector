#include "poly.h"

#include <cmath>
#include <cstdlib>
#include <stdexcept>

Polygon::Polygon(const std::vector<Point>& inp_points, 
                 const std::set<int>& obj_classes, 
                 PolyType poly_type, float threshold, int priority)
    : poly_type(poly_type), threshold(threshold), priority(priority),
      obj_classes(obj_classes) {
  SetCoord(inp_points);
}

Polygon::Polygon(const std::vector<float>& inp_points, 
                 const std::set<int>& obj_classes, 
                 PolyType poly_type, float threshold, int priority)
    : poly_type(poly_type), threshold(threshold), priority(priority),
      obj_classes(obj_classes) {
  SetCoord(inp_points);
}

void Polygon::SetCoord(const std::vector<Point>& inp_points) {
  if (inp_points.size() < 4) {
    throw std::runtime_error("Not enough points");
  }
  vertices_.clear();
  min_x_ = max_x_ = inp_points[0].x;
  min_y_ = max_y_ = inp_points[0].y;
  for (const auto& pt : inp_points) {
    vertices_.emplace_back(pt.x, pt.y);
    min_x_ = std::min(min_x_, pt.x);
    max_x_ = std::max(max_x_, pt.x);
    min_y_ = std::min(min_y_, pt.y);
    max_y_ = std::max(max_y_, pt.y);
  }
}

void Polygon::SetCoord(const std::vector<float>& coord) {
  vertices_.clear();
  if (coord.size() % 2 != 0) {
    throw std::runtime_error("Invalid coordinate array size");
  }
  if (coord.size() < 8) {
    throw std::runtime_error("Not enough points");
  }
  min_x_ = max_x_ = coord[0];
  min_y_ = max_y_ = coord[1];
  for (size_t i = 0; i < coord.size(); i += 2) {
    vertices_.emplace_back(coord[i], coord[i + 1]);
    min_x_ = std::min(min_x_, coord[i]);
    max_x_ = std::max(max_x_, coord[i]);
    min_y_ = std::min(min_y_, coord[i + 1]);
    max_y_ = std::max(max_y_, coord[i + 1]);
  }
}

float Polygon::GetMinX() const { return min_x_; }
float Polygon::GetMaxX() const { return max_x_; }
float Polygon::GetMinY() const { return min_y_; }
float Polygon::GetMaxY() const { return max_y_; }

Intersection Polygon::GetIntersection(const VBox& box) const {
  Intersection out(this->GetPoints());
  out = out.CutByEdge(EdgeType::kLeft, box);
  out = out.CutByEdge(EdgeType::kRight, box);
  out = out.CutByEdge(EdgeType::kBottom, box);
  out = out.CutByEdge(EdgeType::kTop, box);
  return out;
}

double Polygon::GetSIntersection(const VBox& box) const {
  return GetIntersection(box).GetS();
}

const std::vector<Point>& Polygon::GetPoints() const { return vertices_; }
