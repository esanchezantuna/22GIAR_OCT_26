#include <filesystem>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "engine/metrics.h"
#include "engine/globals.h"

struct image_data{
  int width, height, nChannels;
  unsigned char* data;
};

std::atomic<bool> running;

image_data load_image(const char* path) {
  MetricsScope metrics("Load Image");
  image_data img_data;
  img_data.data = stbi_load(path, &img_data.width, &img_data.height, &img_data.nChannels, 0);
  if (img_data.data)
    std::cout << "Image loaded correctly " << path << " " << img_data.width << " x " << img_data.height << ", " << img_data.nChannels << std::endl;
  else
    std::cout << "Error loading image" << path << std::endl;

  return img_data;
}

void delete_image(const char* path) {
  MetricsScope metrics("Load Image");
  std::filesystem::remove(std::filesystem::path(path));
}

int save_image(image_data img_data, const char* path) {
  MetricsScope metrics("Save");
  const int error = stbi_write_png(path, img_data.width, img_data.height, img_data.nChannels, img_data.data, img_data.width * img_data.nChannels);
  if (error == 0)
    std::cout << error << "Error saving image" << path << std::endl;
  else
    std::cout << "Processed Image Saved " << path << " - " << img_data.width << "x" << img_data.height << "," << img_data.nChannels << std::endl;

  return error;
}

void resize_image_half(image_data &img_data) {
  MetricsScope metrics("Resize");
  int width = img_data.width / 2;
  int height = img_data.height / 2;

  size_t buffer_size = width * height * img_data.nChannels;
  int old_stride = img_data.width * img_data.nChannels;
  int new_stride = width * img_data.nChannels;

  auto buffer = static_cast<unsigned char* >(malloc(buffer_size));
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int r = 0;
      int g = 0;
      int b = 0;
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
          r += img_data.data[(row * 2 + i) * old_stride + (col * 2 + j) * img_data.nChannels + 0];
          g += img_data.data[(row * 2 + i) * old_stride + (col * 2 + j) * img_data.nChannels + 1];
          b += img_data.data[(row * 2 + i) * old_stride + (col * 2 + j) * img_data.nChannels + 2];
        }
      }

      r /= 4;
      g /= 4;
      b /= 4;

      buffer[row * new_stride + col * img_data.nChannels + 0] = r;
      buffer[row * new_stride + col * img_data.nChannels + 1] = g;
      buffer[row * new_stride + col * img_data.nChannels + 2] = b;
    }
  }

  stbi_image_free(img_data.data);
  img_data.data = buffer;
  img_data.height = height;
  img_data.width = width;

  std::this_thread::sleep_for(std::chrono::milliseconds(0));   //simulate latency
}

void rotate_image_90CW(image_data& img_data) {
  MetricsScope metrics("Rotate");
  int width = img_data.height;
  int height = img_data.width;

  size_t buffer_size = width * height * img_data.nChannels;
  int old_stride = img_data.width * img_data.nChannels;
  int new_stride = width * img_data.nChannels;

  auto buffer = static_cast<unsigned char*>(malloc(buffer_size));
  for (int row = 0; row < img_data.height; ++row) {
    for (int col = 0; col < img_data.width; ++col) {
      unsigned char r = img_data.data[row * old_stride + col * img_data.nChannels + 0];
      unsigned char g = img_data.data[row * old_stride + col * img_data.nChannels + 1];
      unsigned char b = img_data.data[row * old_stride + col * img_data.nChannels + 2];

      buffer[col * new_stride + (width - row - 1) * img_data.nChannels + 0] = r;
      buffer[col * new_stride + (width - row - 1) * img_data.nChannels + 1] = g;
      buffer[col * new_stride + (width - row - 1) * img_data.nChannels + 2] = b;
    }
  }
  stbi_image_free(img_data.data);
  img_data.data = buffer;
  img_data.height = height;
  img_data.width = width;

  std::this_thread::sleep_for(std::chrono::milliseconds(0));   //simulate latency
}

void grayscale_image(image_data& img_data) {
  MetricsScope metrics("GrayScale");
  int stride = img_data.width * img_data.nChannels;
  for (int row = 0; row < img_data.height; ++row) {
    for (int col = 0; col < img_data.width; ++col) {
      unsigned char r = img_data.data[row * stride + col * img_data.nChannels + 0];
      unsigned char g = img_data.data[row * stride + col * img_data.nChannels + 1];
      unsigned char b = img_data.data[row * stride + col * img_data.nChannels + 2];

      float average = 0.2126 * r + 0.7152 * g + 0.0722 * b;

      img_data.data[row * stride + col * img_data.nChannels + 0] = average;
      img_data.data[row * stride + col * img_data.nChannels + 1] = average;
      img_data.data[row * stride + col * img_data.nChannels + 2] = average;
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(0));   //simulate latency
}

void calc_diff_queue(std::vector<image_data>& queue) {
  MetricsScope metrics("Diff");
  image_data diff;
  diff.width = queue[0].width;
  diff.height = queue[0].height;
  diff.nChannels = queue[0].nChannels;

  size_t buffer_size = diff.width * diff.height * diff.nChannels;
  int stride = diff.width * diff.nChannels;

  auto buffer = static_cast<unsigned char*>(malloc(buffer_size));
  diff.data = buffer;

  for (int row = 0; row < diff.height; ++row) {
    for (int col = 0; col < diff.width; ++col) {

      std::vector<unsigned char> r;
      for (const auto& image : queue) {
        r.push_back(image.data[row * stride + col * diff.nChannels + 0]);
      }

      const auto [r_min, r_max] = std::minmax_element(begin(r), end(r));

      int final_diff = *r_max - *r_min == 0 ? 0 : *r_max;
      diff.data[row * stride + col * diff.nChannels + 0] = final_diff;
      diff.data[row * stride + col * diff.nChannels + 1] = final_diff;
      diff.data[row * stride + col * diff.nChannels + 2] = final_diff;
    }
  }

  // save file
  static int id = 0;
  const std::string base_path = ASSETS_PATH;
  const std::string output_path = base_path + "/output_cpp/diff/diff_" + std::to_string(id++) + ".png";
  int error = save_image(diff, output_path.c_str());
  stbi_image_free(diff.data);

  for (const auto& image : queue) {
    stbi_image_free(image.data);
  }
  queue.clear();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));   //simulate latency
}

void computer_vision_algorithm() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));   //simulate latency
}

void break_loop() {
  MetricsScope metrics("Check for keyboard");
  std::cin.get();
  running = false;
}

void main_loop() {
  MetricsScope metrics("Main Loop");
  std::cout << "Running!!!" << std::endl;

  std::chrono::high_resolution_clock::time_point baseTime = std::chrono::high_resolution_clock::now();
  int minutes_running = 5;

  std::vector<image_data> queue;
  const int length_diff = 4;

  const std::string base_path = ASSETS_PATH;

  running = true;
  auto thread = std::thread(break_loop);
  thread.detach();

  while (running) {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - baseTime);
    if (duration > std::chrono::minutes(minutes_running)) {
      std::cout << "Work is Done" << std::endl;
      break;
    }

    for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
      if (is_regular_file(entry.path())) {
        // load file
        image_data img_data = load_image(entry.path().string().c_str());
        // delete image
        delete_image(entry.path().string().c_str());
        // resize image
        resize_image_half(img_data);
        // rotate image
        rotate_image_90CW(img_data);
        // gray scale image
        grayscale_image(img_data);
        // save file
        const std::string output_path = base_path + "/output_cpp/" + entry.path().filename().string();
        int error = save_image(img_data, output_path.c_str());
        // difference queue
        queue.push_back(img_data);
        if (queue.size() >= length_diff) {
          calc_diff_queue(queue);
        }
        // call the computer vision algorithm
        computer_vision_algorithm();
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }
}

int main(int, char* []) {
  Global::metrics = std::make_unique<MetricsState>();
  Metrics_StartRecording(PROJECT_PATH "metrics.json");

  main_loop();

  Metrics_StopRecording();
  Global::metrics.reset(nullptr);

  return 0;
}