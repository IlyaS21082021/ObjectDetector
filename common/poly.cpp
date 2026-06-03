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

double Polygon::GetS() const {
  if (vertices_.size() < 3) return 0;
  double s = 0.0;
  size_t n = vertices_.size();
  for (size_t i = 0; i < n; ++i) {
    const Point& a = vertices_[i];
    const Point& b = vertices_[(i + 1) % n];
    s += static_cast<double>(a.x) * b.y;
    s -= static_cast<double>(b.x) * a.y;
  }
  return std::abs(s) * 0.5;
}

void Polygon::PushBack(const Point& p) {
  if (vertices_.empty()) {
    min_x_ = max_x_ = p.x;
    min_y_ = max_y_ = p.y;
  } else {
    min_x_ = std::min(min_x_, p.x);
    min_y_ = std::min(min_y_, p.y);
    max_x_ = std::max(max_x_, p.x);
    max_y_ = std::max(max_y_, p.y);
  }
  vertices_.push_back(p);
}

Polygon Polygon::CutByEdge(EdgeType edge, const VBox& box) const {
  Polygon res;
  if (vertices_.empty()) return res;
  Point prev_point = vertices_.back();
  for (const Point& cur_point : vertices_) {
    bool cur_inside = box.PointInsideCheck(cur_point, edge);
    bool prev_inside = box.PointInsideCheck(prev_point, edge);
    if (prev_inside && cur_inside) { //inside -> inside
      res.PushBack(cur_point);
    } else if (prev_inside && !cur_inside) { //inside -> outside
      res.PushBack(box.IntersectionWithLine(prev_point, cur_point, edge));
    } else if (!prev_inside && cur_inside) { //outside -> inside
      res.PushBack(box.IntersectionWithLine(prev_point, cur_point, edge));
      res.PushBack(cur_point);
    }
    // outside -> outside: skip
    prev_point = cur_point;
  }
  return res;
}

Polygon Polygon::GetIntersection(const VBox& box) const {
  Polygon out = *this;
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
