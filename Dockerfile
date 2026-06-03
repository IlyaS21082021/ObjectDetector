FROM gcc:16-trixie

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    git \
    libopencv-dev \
    libyaml-cpp-dev \
    wget \
    ca-certificates \

    libx11-xcb1 \
    libxcb-xinerama0 \
    libxrender1 \
    libxi6 \
    libice6 \
    libsm6

WORKDIR /home
RUN wget https://github.com/microsoft/onnxruntime/releases/download/v1.26.0/onnxruntime-linux-x64-1.26.0.tgz
RUN tar -xzf onnxruntime-linux-x64-1.26.0.tgz
RUN rm onnxruntime-linux-x64-1.26.0.tgz
RUN git clone https://github.com/IlyaS21082021/ObjectDetector.git
RUN mkdir /usr/include/onnx
RUN cp -r onnxruntime-linux-x64-1.26.0/include/* /usr/include/onnx
RUN mkdir /usr/lib/onnx
RUN cp -r onnxruntime-linux-x64-1.26.0/lib/libonnxruntime.so.1.26.0 /usr/lib/onnx
WORKDIR /usr/lib/onnx
RUN ln -s libonnxruntime.so.1.26.0 libonnxruntime.so
RUN ln -s libonnxruntime.so.1.26.0 libonnxruntime.so.1
WORKDIR /home/ObjectDetector/client/
WORKDIR /home/ObjectDetector/client/build
RUN cmake ..
RUN make
WORKDIR /home/ObjectDetector/server/build
RUN cmake ..
RUN make
WORKDIR /home
RUN rm -r onnxruntime-linux-x64-1.26.0
