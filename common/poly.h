#ifndef POLY_H_
#define POLY_H_

#include <set>
#include <utility>
#include <vector>

#include "point.h"
#include "vbox.h"

enum class PolyType { kInclude, kExclude };
/**
 * @brief Class describing polygon
 */
class Polygon {
 public:
  float threshold = 0.5;
  int priority = 1;
  PolyType poly_type = PolyType::kInclude;
  std::set<int> obj_classes;

  Polygon() = default;

  /**
   * @brief Constructor of the class
   * 
   * @param inp_points Vector of points for polygon vertices.
   * @param obj_classes Set of numbers corresponding object of classes 
   *                    the polygon is applied to.
   * @param poly_type Parameter defining include/exclude polygon.
   * @param threshold Number which set relation of the intersection area
   *                  of a box and a polygon to the box area for display the 
   *                  box.
   * @param priority Priority of the polygon in box process.
   */
  Polygon(const std::vector<Point>& inp_points, 
          const std::set<int>& obj_classes,
          PolyType poly_type = PolyType::kInclude, float threshold = 0.5,
          int priority = 1);

/**
   * @brief Constructor of the class
   * 
   * @param inp_points Vector of separate coordinates x and y 
   *                   for polygon vertices.
   * @param obj_classes Set of numbers corresponding object of classes 
   *                    the polygon is applied to.
   * @param poly_type Parameter defining include/exclude polygon.
   * @param threshold Number which set relation of the intersection area
   *                  of a box and a polygon to the box area for display the 
   *                  box.
   * @param priority Priority of the polygon in box process.
   */
  Polygon(const std::vector<float>& inp_points, 
          const std::set<int>& obj_classes,
          PolyType poly_type = PolyType::kInclude, float threshold = 0.5,
          int priority = 1);

  /**
   * @breif Function to set polygon vertices coordinates
   * 
   * @param coord Vector of polygon vertices x and y coordinates
   */
  void SetCoord(const std::vector<float>& coord);

  /**
   * @breif Function to set polygon vertices coordinates
   * 
   * @param coord Vector of polygon vertices coordinates in Point structure
   */
  void SetCoord(const std::vector<Point>& inp_points);

  /**
   * @brief Function returns min x value of polygon vertices
   * 
   * @return Min x value of polygon vertices
   */
  float GetMinX() const;

  /**
   * @brief Function returns max x value of polygon vertices
   * 
   * @return Max x value of polygon vertices
   */
  float GetMaxX() const;

  /**
   * @brief Finction returns min y value of polygon vertices
   * 
   * @return Min y value of polygon vertices
   */
  float GetMinY() const;

  /**
   * @brief Function returns max y value of polygon vertices
   * 
   * @return Max y value of polygon vertices
   */
  float GetMaxY() const;

  /**
   * @brief Function returns area of polygon
   * 
   * @return Area of polygon
   */
  double GetS() const;

  /**
   * @brief Function returns area of intersection of polygon and box
   * 
   * @param box Box which intersects the polygon
   * @return Area of intersection
   */
  double GetSIntersection(const VBox& box) const;

  /**
   * @brief Function to add vertex to polygon
   * 
   * @param p Point for new vertex
   */
  void PushBack(const Point& p);

  /**
   * @brief Function to get access to polygon verices
   * 
   * @return Reference to vertices vector 
   */
  const std::vector<Point>& GetPoints() const;

 private:
  float min_x_ = 0;
  float max_x_ = 0;
  float min_y_ = 0;
  float max_y_ = 0;
  std::vector<Point> vertices_;

  /**
   * @brief Function to cut polygon by edge of a box
   * 
   * @param edge Type of box edge which cut the polygon
   * @param box The box which intersects the polygon
   * @return New cut polygon
   */
  Polygon CutByEdge(EdgeType edge, const VBox& box) const;

  /**
   * @brief Function to obtain the intersection of a polygon and a box
   * 
   * @param box The box which intersects the polygon
   * @return New polygon which is intersaction of the box and the polygon
   */
  Polygon GetIntersection(const VBox& box) const;
};

#endif  // POLY_H_
