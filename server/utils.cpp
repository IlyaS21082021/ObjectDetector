#include "utils.h"

std::vector<VBox> BoxFilter(const std::vector<VBox>& boxes,
                            const std::vector<Polygon>& polygons) {
  std::vector<VBox> res;
  for (const auto& b : boxes) {
    int high_priority_idx = -1;
    for (size_t i = 0; i < polygons.size(); ++i) {
      // Quick bounding-box rejection
      if (b.x2 < polygons[i].GetMinX() || b.x1 > polygons[i].GetMaxX() ||
          b.y2 < polygons[i].GetMinY() || b.y1 > polygons[i].GetMaxY()) {
        continue;
      }
      //select high priority polygon for cur box which intersects with polygones
      if (polygons[i].obj_classes.contains(b.obj_class)) {
        if (polygons[i].GetSIntersection(b) / b.GetS() >=
            polygons[i].threshold) {
          if (high_priority_idx == -1 ||
              polygons[i].priority > polygons[high_priority_idx].priority) {
            high_priority_idx = i;
          }
        }
      }
    }
    if (high_priority_idx >= 0 &&
        polygons[high_priority_idx].poly_type == PolyType::kInclude) {
      res.push_back(b);
    }
  }
  return res;
}
