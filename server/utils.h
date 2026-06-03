#ifndef UTILS_H_
#define UTILS_H_

#include <vector>

#include "poly.h"
#include "vbox.h"

/**
 * @brief Function with the main logic which defiens show box with object or not
 * 
 * @param boxes The vector of unique boxes with certain classes of objects
 * @param polygons The vector of polygons that defines which box should be displayed
 * @return Vector if boxes that should be displayed
 */
std::vector<VBox> BoxFilter(const std::vector<VBox>& boxes,
                            const std::vector<Polygon>& polygons);

#endif  // UTILS_H_
