#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "aux.h"

TEST(CheckInputData, CheckImageRead) {
  YAML::Node config = YAML::LoadFile("../test_config.yaml");
  auto encoded_image = LoadImage("../" + config["image"].as<std::string>());
  EXPECT_EQ(encoded_image.size(), 115139);
}

TEST(CheckInputData, CheckPolygonRead) {
  YAML::Node config = YAML::LoadFile("../test_config.yaml");
  auto polygons = LoadPolygons(config);
  EXPECT_EQ(polygons.size(), 2);

  EXPECT_EQ(polygons[0].GetPoints().size(), 5);
  EXPECT_NEAR(polygons[0].threshold_, 0.21, 1e-4);
  EXPECT_EQ(polygons[0].priority_, 3);
  EXPECT_EQ(polygons[0].poly_type_, PolyType::kExclude);
  EXPECT_EQ(polygons[0].obj_classes_.size(), 6);
}

TEST(CheckPolygonProc, CheckPolygonCreation) {
  Polygon poly;
  
}

TEST(CheckPolygonProc, CheckPolygonIntersection) {
  Polygon poly();

  double s = poly.GetSIntersection()
}