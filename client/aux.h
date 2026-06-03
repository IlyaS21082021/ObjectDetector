#ifndef AUX_H_
#define AUX_H_

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

#include "ObjectFinder.grpc.pb.h"
#include "poly.h"

/**
 * @brief Function to load polygons data from config.yaml
 * 
 * @param config Object of YAML::Node type which contains input data
 */
std::vector<Polygon> LoadPolygons(const YAML::Node& config);

/**
 * @brief Function to load image from filesystem to application
 * 
 * @param img_path String path of image in file system
 */
std::vector<uchar> LoadImage(const std::string& img_path);

/**
 * @brief Function to load data in request
 * 
 * @param req Request, contains loaded data
 * @param polygons Vector of data of polygons
 */
void LoadPolygonsToReq(ObjectFinder::ImageRequest& req,
                       const std::vector<Polygon>& polygons);

#endif  // AUX_H_
