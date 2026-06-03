#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <grpcpp/grpcpp.h>

#include "service.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Port is not set\n";
    return 1;
  }
  const std::string kServerAddress = "0.0.0.0:" + std::string(argv[1]);

  ObjectFinderService service;
  grpc::ServerBuilder builder;
  int selected_port = 0;
  builder.AddListeningPort(kServerAddress, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
  if (!server) {
    throw std::runtime_error("Failed to start server");
  }

  std::cout << "Server started on " << kServerAddress <<"\n";
  server->Wait();
  return 0;
}
