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
  EXPECT_NEAR(polygons[0].threshold, 0.21, 1e-4);
  EXPECT_EQ(polygons[0].priority, 3);
  EXPECT_EQ(polygons[0].poly_type, PolyType::kExclude);
  EXPECT_EQ(polygons[0].obj_classes.size(), 6);
}