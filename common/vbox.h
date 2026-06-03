#ifndef VBOX_H_
#define VBOX_H_

#include "point.h"

enum class EdgeType { kLeft, kRight, kBottom, kTop };

/**
 * @brief  Class describing box
 */
struct VBox {
  float x1;
  float y1;
  float x2;
  float y2;
  int obj_class;

  /**
   * @brief Constuctor of the box
   * 
   * @param xc Coordinate x of the box center
   * @param yx Coordinate y of the box center
   * @param w width of the box
   * @param h hieght of the box
   * @param obj_class Class of the object the box frames
   */
  VBox(float xc, float yc, float w, float h, int obj_class)
      : x1(xc - w/2), y1(yc - h/2), x2(xc + w/2),
        y2(yc + h/2), obj_class(obj_class) {}

  /**
   * @brief  Function to check if the point inside of the box
   * 
   * @param x coordinate x of the point
   * @param y coorinate y of the point
   * @return True if the point inside of the box
   */
  bool Contains(float x, float y) const {
    return (x1 <= x) && (x <= x2) && (y1 <= y) && (y <= y2);
  }

  /**
   * @brief Function to check a point location behind of the edge
   *        of the box or out of the box
   * 
   * @param p Point that should be checked
   * @param edge Type of the edge of the box to check the point
   * @return False if point outside the box, True if it can be inside
   */
  bool PointInsideCheck(const Point& p, EdgeType edge) const {
    switch (edge) {
      case EdgeType::kLeft:   return p.x >= x1;
      case EdgeType::kRight:  return p.x <= x2;
      case EdgeType::kBottom: return p.y >= y1;
      case EdgeType::kTop:    return p.y <= y2;
    }
    return false;
  }

  /**
   * @brief Function for calulate intersection of a line and edge of a box
   * 
   * @param p1 First point of the line
   * @param p2 Second point of the line
   * @param edge Type of edge of the box
   * @return point of intersection
   */
  Point IntersectionWithLine(const Point& p1, const Point& p2,
                               EdgeType edge) const {
    Point res{};
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    switch (edge) {
      case EdgeType::kLeft:
        res.x = x1;
        res.y = p1.y + dy * (x1 - p1.x) / dx;
        break;
      case EdgeType::kRight:
        res.x = x2;
        res.y = p1.y + dy * (x2 - p1.x) / dx;
        break;
      case EdgeType::kBottom:
        res.y = y1;
        res.x = p1.x + dx * (y1 - p1.y) / dy;
        break;
      case EdgeType::kTop:
        res.y = y2;
        res.x = p1.x + dx * (y2 - p1.y) / dy;
        break;
    }
    return res;
  }

  /**
   * @brief Function to recalculate box coordinates in accordance with
   *        scale coefficients
   * 
   * @param sx x scale coefficient
   * @param sy y scale coefficient
   */
  void ScaleData(float sx, float sy) {
    x1 *= sx;
    x2 *= sx;
    y1 *= sy;
    y2 *= sy;
  }

  /**
   * @brief Function for calculation of box area
   * 
   * @return box area
   */
  double GetS() const { return (x2 - x1)*(y2 - y1); }
};

#endif  // VBOX_H_
