
#include <array>
#include <algorithm>

#include <opencv2/opencv.hpp>

uint8_t op(double v)
{
    return uint8_t(v * 255.0);
}

std::array<uint8_t, 3> get_colour(double v, double vmin, double vmax)
{
    std::array<double, 3> c = {1.0, 1.0, 1.0}; // white
    double dv;

    if (v < vmin)
        v = vmin;
    if (v > vmax)
        v = vmax;
    dv = vmax - vmin;

    if (v < (vmin + 0.25 * dv))
    {
        c[0] = 0;
        c[1] = 4 * (v - vmin) / dv;
    }
    else if (v < (vmin + 0.5 * dv))
    {
        c[0] = 0;
        c[2] = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
    }
    else if (v < (vmin + 0.75 * dv))
    {
        c[0] = 4 * (v - vmin - 0.5 * dv) / dv;
        c[2] = 0;
    }
    else
    {
        c[1] = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
        c[2] = 0;
    }

    std::array<uint8_t, 3> color;
    std::transform(c.begin(), c.end(), color.begin(), op);

    return color;
}

void populate_colormap(std::array<std::array<uint8_t, 3>, 256> &jet)
{
    double index = 0;
    for (auto &c : jet)
    {
        c = get_colour(index, 0, jet.size());
        index += 1;
    }
}

void convert_image(const cv::Mat &input_image, cv::Mat &output_image, const uint16_t vmin, const uint16_t vmax, const std::array<std::array<uint8_t, 3>, 256> &jet)
{
    // Convert a CV_16UC1 to a CV_8C3
    output_image = cv::Mat(input_image.rows, input_image.cols, CV_8UC3);

#pragma omp parallel for
    for (int r = 0; r < input_image.rows; r++)
    {
#pragma omp parallel for
        for (int c = 0; c < input_image.cols; c++)
        {
            uint16_t pixel = input_image.at<uint16_t>(r, c);
            if (pixel == 0)
            {
                output_image.at<cv::Vec3b>(r, c) = {0, 0, 0};
            }
            else
            {
                size_t ind = std::round(255 * (pixel - vmin) / (vmax - vmin));
                if (ind >= jet.size())
                {
                    ind = jet.size() - 1;
                }
                cv::Vec3b val{jet[ind][2], jet[ind][1], jet[ind][0]};
                output_image.at<cv::Vec3b>(r, c) = val;
            }
        }
    }
    return;
}