#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <cmath>
#include <vector>

#include "point.h"
#include "vbox.h"

/**
 * @brief Class describing the intersection of a poligon and a box
 */
struct Intersection {
  std::vector<Point> points;

  /**
   * @brief Default constructor
   */
  Intersection() = default;

  /**
   * @brief Constructor with set of points
   * 
   * @param vpoints Vector of points that form intersection 
   */
  Intersection(const std::vector<Point>& vpoints) {
    points = vpoints;
  }

  /**
   * @brief Function returns area of intersection
   * 
   * @return Area of polygon
   */
  double GetS() const {
    if (points.size() < 3) return 0;
    double s = 0.0;
    size_t n = points.size();
    for (size_t i = 0; i < n; ++i) {
      const Point& a = points[i];
      const Point& b = points[(i + 1) % n];
      s += static_cast<double>(a.x) * b.y;
      s -= static_cast<double>(b.x) * a.y;
    }
    return std::abs(s) * 0.5;
  }

  /**
   * @brief Function the construct intersection geometry from 
   *        the cut polygon by edge of a box
   * 
   * @param edge Type of box edge which cut the polygon
   * @param box The box which intersects the polygon
   * @return New intersection
   */
  Intersection CutByEdge(EdgeType edge, const VBox& box) const {
    Intersection res;
    if (points.empty()) return res;
    Point prev_point = points.back();
    for (const Point& cur_point : points) {
      bool cur_inside = box.PointInsideCheck(cur_point, edge);
      bool prev_inside = box.PointInsideCheck(prev_point, edge);
      if (prev_inside && cur_inside) { //inside -> inside
        res.points.push_back(cur_point);
      } else if (prev_inside && !cur_inside) { //inside -> outside
        res.points.push_back(box.IntersectionWithLine(prev_point, cur_point, edge));
      } else if (!prev_inside && cur_inside) { //outside -> inside
        res.points.push_back(box.IntersectionWithLine(prev_point, cur_point, edge));
        res.points.push_back(cur_point);
      }
      // outside -> outside: skip
      prev_point = cur_point;
    }
    return res;
  }
};

#endif