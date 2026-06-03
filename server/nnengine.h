#ifndef NNENGINE_H_
#define NNENGINE_H_

#include <onnxruntime_cxx_api.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "vbox.h"

/**
 * @brief Class to work with onnx model
 */
class NNEngine {
 public:
 /**
  * @brief  Consturctor of the class
  * 
  * @param model_path Path to the model file
  * @param width Width of the images the model should process
  * @param height Height of the image the model should process
  */
  NNEngine(const std::string& model_path, int width, int height);

  /**
   * @brief Function which run onnx model for the image
   * 
   * @param image Data of the loaded image
   * @return True if image processed whithout errors
   */
  bool ProcessImage(const cv::Mat& image);

  /**
   * @brief Function to get access to x coordinates of the center
   *        of the box which frame an object on the image
   * 
   * @return pointer to the x coordinates
   */
  const float* GetXP();

  /**
   * @brief Function to get access to y coordinates of the center
   *        of the box which frame an object on the image
   * 
   * @return pointer to the y coordinates
   */
  const float* GetYP();

  /**
   * @brief Function to get access to width 
   *        of the box which frame an object on the image
   * 
   * @return pointer to the width coordinates
   */
  const float* GetWidthP();

  /**
   * @brief Function to get access to height
   *        of the box which frame an object on the image
   * 
   * @return pointer to the height coordinates
   */
  const float* GetHeightP();

  /**
   * @brief Function to get access to values corresponding the probability
   *        of the certain class of the framed object
   * 
   * @return pointer to the values
   */
  const float* GetObjTypeValuesP(int obj_type);

  /**
   * @brief Function to obtain boxes of certain classes of objects
   * 
   * @param obj_type The set of object classes the frame which should be 
   *                  obtained of.
   * @param min_obj_type_score Min probability of the certain class of 
   *                           object in the box that should be considered
   *                           as the object was found.
   * @return Vector of the boxes corresponding to the classes of the objects
   *         in obj_types set
   */
  const std::vector<VBox>& GetBoxes(const std::set<int>& obj_types,
                                    float min_obj_type_score);

 private:
  int input_width_;
  int input_height_;

  Ort::Env env_;
  Ort::SessionOptions session_options_;
  std::unique_ptr<Ort::Session> session_;

  std::string input_name_str_;
  std::string output_name_str_;
  std::array<int64_t, 4> input_shape_;
  Ort::MemoryInfo memory_info_;

  std::vector<float> input_tensor_values_;
  size_t channel_size_;

  std::vector<Ort::Value> output_tensors_;
  float sx_;
  float sy_;
  size_t objects_count_;

  std::vector<VBox> res_boxes_;

  /**
   * @brief Function deletes doublicates of the box for the same object
   * 
   * @param data All found boxes
   * @return Vector of unique boxes
   */
  std::vector<VBox> DelDoubleObjects(std::vector<std::vector<VBox>>& data);
};

#endif  // NNENGINE_H_
