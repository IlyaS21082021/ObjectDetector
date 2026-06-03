#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

#include "ObjectFinder.grpc.pb.h"
#include "aux.h"

int main() {
  YAML::Node config = YAML::LoadFile("config.yaml");

  auto polygons = LoadPolygons(config);
  auto encoded_image = LoadImage(config["image"].as<std::string>());

  auto channel = grpc::CreateChannel("localhost:" + config["port"].as<std::string>(),
                                     grpc::InsecureChannelCredentials());
  std::unique_ptr<ObjectFinder::ObjectFinderServiceGRPC::Stub> stub =
      ObjectFinder::ObjectFinderServiceGRPC::NewStub(channel);

  ObjectFinder::ImageRequest request;
  request.set_image(reinterpret_cast<const char*>(encoded_image.data()),
                    encoded_image.size());
  LoadPolygonsToReq(request, polygons);

  ObjectFinder::ImageResponse response;
  grpc::ClientContext context;
  //call server remote procedure 
  grpc::Status status = stub->SelectObjects(&context, request, &response);
  if (!status.ok()) {
    std::cerr << "RPC failed: " << status.error_code() << " "
              << status.error_message() << "\n";
    return 1;
  }

  const std::string& img_data = response.image();
  std::vector<uchar> result_bytes(img_data.begin(), img_data.end());
  cv::Mat result_image = cv::imdecode(result_bytes, cv::IMREAD_COLOR);
  if (result_image.empty()) {
    std::cerr << "Cannot decode response image\n";
    return 1;
  }

  cv::imshow("image", result_image);
  cv::waitKey(0);
  return 0;
}
