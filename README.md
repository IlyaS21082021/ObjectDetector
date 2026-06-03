1. Project Structure
The project consists of four folders:
- client
- common
- grpc
- server.
client: Contains files for building the client-side portion of the application and a folder with tests.
common: Contains files used by the client and server portions of the application.
grpc: A folder with files obtained by processing the .proto file to implement the grpc engine.
This folder also contains the file generation script.
server: Contains files for building the server-side portion of the application and a folder with tests.

2. Build
The client and server portions of the application are built separately, each in its own folder.

2.1. Build the client portion.
In the client folder, create a "build" folder. From there, run the CMakeLists.txt file, then "make." The application
will be built and copied to the client folder.

2.2. Build the server portion.
In the server folder, create a "build" folder. Run the CMakeLists.txt file from there, then make. The application
will be built and copied to the server folder.

3. Input Data
The input data is specified in a yaml file. The file should be located next to the client-side executable
and named config.yaml. The config.yaml file specifies the image file and polygons with
parameters. Example data structure:
port: 22000
image: test.jpg
polygons:
  - id: p1
    vertices:
      - { x: 58, y: 200 }
      - { x: 124, y: 300 }
      - { x: 300, y: 500 }
      - { x: 400, y: 250 }
      - { x: 100, y: 160 }
    threshold: 0.21
    priority: 3
    type: exclude
    obj_classes: [0,1,2,5,7,8]

  - id: p2
    vertices:
      - { x: 600, y: 140 }
      - { x: 550, y: 200 }
      - { x: 700, y: 300 }
      - { x: 1000, y: 450 }
      - { x: 850, y: 260 }
    threshold: 0.3
    priority: 2
    type: exclude
    obj_classes: [0,1,2,4,6]

The "polygons" block defines an array of polygons.
The "vertices" block defines an array of vertices with coordinates. There must be at least 4 vertices.
"threshold" - specifies the minimum ratio of the intersection area of ​​a given polygon with the object's box in the image to the area of ​​that box for displaying the object's box.
"priority" - polygon priority.
"type" - polygon type: include/exclude
"obj_classes" - array of object classes with which the polygon interacts.
"port" - port number where the server is located.
The coordinates of the polygon's vertices are defined based on the origin being in the upper left corner.

4. Server Startup
The server is started with the port number parameter.

5. Tests
Tests for the client and server are compiled and run separately. To build tests for the client,
you need to create a build folder in client/tests and run CMakeLists.txt from there, located
in the client/tests folder. Then, run make. This will create a file called client_tests.
To build tests for the server, you need to create a build folder in server/tests and run CMakeLists.txt from there, located in the server/tests folder. Then, run make. This will create a file called server_tests.
