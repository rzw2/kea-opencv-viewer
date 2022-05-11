#include <iostream>
#include <vector>
#include <exception>
#include <chrono>
#include <thread>

#include <chronoptics/tof/kea_camera.hpp>
#include <chronoptics/tof/kea.hpp>
#include <chronoptics/tof/user_config.hpp>
#include <chronoptics/tof/camera_config.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "cxxopts.hpp"
#include "colormap.hpp"

namespace tof = chronoptics::tof;
using namespace std::chrono_literals;

void get_frame(std::vector<tof::Data> &frames, const tof::FrameType frame_type, tof::Data &out_frame)
{
    for (tof::Data &frame : frames)
    {
        if (frame.frame_type() == frame_type)
        {
            out_frame = std::move(frame);
            break;
        }
    }
    return;
}

int main(int argc, char **argv)
{
    cxxopts::Options options("Kea on-camera display",
                             "Display the output of Kea camera to the HDMI port");

    cv::String intensity_name = "Intensity";
    cv::String rad_name = "Radial";
    cv::String bgr_name = "BGR";
    bool bgr = false;
    float dmax;
    float fps;

    options.add_options()("h, help", "Help")("bgr", "Display the colour image", cxxopts::value<bool>(bgr)->default_value("false"))("dmax", "Maximum distance", cxxopts::value<float>(dmax)->default_value("15.0"))("fps", "Depth frames per second", cxxopts::value<float>(fps)->default_value("20.0"));

    auto result = options.parse(argc, argv);

    if (result.count("h") || result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::vector<tof::FrameType> types = {tof::FrameType::RADIAL, tof::FrameType::INTENSITY};
    if (bgr)
    {
        types.push_back(tof::FrameType::BGR);
    }

    // The way to color the depth image with a JET colour map
    std::array<std::array<uint8_t, 3>, 256> jet;
    populate_colormap(jet);

    try
    {

        auto proc = tof::ProcessingConfig();
        proc.set_calibration_enabled(true);
        proc.set_intensity_scale(5.0f);

        tof::EmbeddedKeaCamera cam(tof::ProcessingConfig{});
        // tof::KeaCamera cam(tof::ProcessingConfig{});

        // Configure the camera
        tof::UserConfig user{};
        user.set_fps(fps);
        user.set_integration_time(tof::IntegrationTime::MEDIUM);
        user.set_max_distance(dmax);
        user.set_environment(tof::ImagingEnvironment::INSIDE);
        user.set_strategy(tof::Strategy::BALANCED);

        auto config = user.to_camera_config(cam);
        for (size_t n = 0; n < config.frame_size(); n++)
        {
            // Flip the output to be displayed correctly with 1/4" Mounting hole
            config.set_flip(n, true);
        }

        // Generate the ToF ISP filtering settings
        proc = config.default_processing();
        proc.set_intensity_scale(5.0);

        // Set the camera settings
        cam.set_camera_config(config);
        cam.set_process_config(proc);

        // Check bgr is avaliable on this camera.
        bool bgr_avaliable = false;
        auto stream_list = cam.get_stream_list();
        for (auto &stream : stream_list)
        {
            if (stream.frame_type() == tof::FrameType::BGR)
            {
                bgr_avaliable = true;
                break;
            }
        }
        if (bgr)
        {
            bgr = bgr_avaliable;
        }

        size_t nstreams = tof::select_streams(cam, types);

        if (bgr)
        {
            cv::namedWindow(bgr_name, cv::WINDOW_AUTOSIZE);
        }
        cv::namedWindow(intensity_name, cv::WINDOW_AUTOSIZE);
        cv::namedWindow(rad_name, cv::WINDOW_AUTOSIZE);

        cam.start();
        cv::Mat disp_img;
        cv::Mat rad_disp;
        cv::Mat rgb_disp;

        while (cam.is_streaming())
        {
            std::vector<tof::Data> frames = cam.get_frames();
            std::cout << "Frame count: " << frames.at(0).frame_count() << std::endl;

            // std::this_thread::sleep_for(200ms);
            for (auto &frame : frames)
            {
                if (frame.frame_type() == tof::FrameType::RADIAL)
                {
                    cv::Mat rad_img(frame.rows(), frame.cols(), CV_16UC1, frame.data());
                    convert_image(rad_img, rad_disp, 0, 65535, jet);
                    // rad_img.convertTo(rad_disp, CV_8UC1, 1.0 / 255.0);

                    cv::imshow(rad_name, rad_disp);
                }
                else if (frame.frame_type() == tof::FrameType::BGR)
                {
                    if (bgr)
                    {
                        cv::Mat bgr_img(frame.rows(), frame.cols(), CV_8UC3, frame.data());
                        // Without copying the program crashes ...
                        bgr_img.copyTo(rgb_disp);
                        cv::imshow(bgr_name, rgb_disp);
                    }
                }
                else if (frame.frame_type() == tof::FrameType::INTENSITY)
                {
                    cv::Mat intensity_img(frame.rows(), frame.cols(), CV_8UC1, frame.data());
                    cv::imshow(intensity_name, intensity_img);
                }
            }

            // If escape is selected then close
            if (cv::waitKey(10) == 27)
            {
                cam.stop();
            }
        }
    }
    catch (std::exception &e)
    {
        cv::destroyAllWindows();
        std::cout << e.what() << std::endl;
        return -1;
    }
    cv::destroyAllWindows();
    return 0;
}