#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "nnengine.h"
#include "poly.h"
#include "vbox.h"
#include "utils.h"

NNEngine nn_engine("../../yolo11n.onnx", 640, 640);
std::vector<VBox> unique_boxes;

TEST(CheckServer, CheckProcessImage) {
  cv::Mat image = cv::imread("../test.jpg");
  bool res = nn_engine.ProcessImage(image);
  EXPECT_TRUE(res);
}

TEST(CheckInputData, CheckUniqueBoxes) {
  unique_boxes = nn_engine.GetBoxes({2,7}, 0.55f);
  EXPECT_EQ(unique_boxes.size(), 2);
  float s = (unique_boxes[0].x2 - unique_boxes[0].x1)*
            (unique_boxes[0].y2 - unique_boxes[0].y1);
  EXPECT_NEAR(s, 118056.12, 1e-2);

  s = (unique_boxes[1].x2 - unique_boxes[1].x1)*
      (unique_boxes[1].y2 - unique_boxes[1].y1);
  EXPECT_NEAR(s, 139317.26, 1e-2);
}

TEST(CheckInputData, CheckResultBoxes) {
  std::vector<Polygon> polygons;
  std::vector<float> points1 = {100, 300, 
                                100, 500, 
                                400, 500, 
                                400, 300};
  std::vector<float> points2 = {600, 300, 
                                600, 500, 
                                1000, 500, 
                                1000, 300};
  std::set<int> ss = {0,1,2,7};
  polygons.emplace_back(points1, ss);
  polygons.emplace_back(points2, ss, PolyType::kExclude, 0.5, 1);
  polygons.emplace_back(points2, ss, PolyType::kInclude, 0.1, 2);
  auto filtered_boxes = BoxFilter(unique_boxes, polygons);
  EXPECT_EQ(filtered_boxes.size(), 1);
  EXPECT_NEAR(filtered_boxes[0].x1, 659.72, 1e-2);
}