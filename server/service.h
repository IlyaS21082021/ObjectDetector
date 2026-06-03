#ifndef SERVICE_H_
#define SERVICE_H_

#include <vector>

#include <grpcpp/grpcpp.h>

#include "ObjectFinder.grpc.pb.h"
#include "nnengine.h"

class ObjectFinderService final
    : public ObjectFinder::ObjectFinderServiceGRPC::Service {
 public:
  ObjectFinderService();

  grpc::Status SelectObjects(grpc::ServerContext* context,
                             const ObjectFinder::ImageRequest* request,
                             ObjectFinder::ImageResponse* response) override;

 private:
  NNEngine nn_engine_;
};

#endif  // SERVICE_H_
