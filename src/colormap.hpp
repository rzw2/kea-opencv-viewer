#ifndef _COLORMAP_HPP_
#define _COLORMAP_HPP_

#include <array>
#include <opencv2/opencv.hpp>

void populate_colormap(std::array<std::array<uint8_t, 3>, 256> &jet);
void convert_image(const cv::Mat &input_image, cv::Mat &output_image, const uint16_t vmin, const uint16_t vmax, const std::array<std::array<uint8_t, 3>, 256> &jet);

#endif