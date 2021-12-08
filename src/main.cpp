#include <iostream>
#include <vector>
#include <exception>

#include <chronoptics/tof/kea_camera.hpp>
#include <chronoptics/tof/kea.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

namespace tof = chronoptics::tof;

int main(int argc, char **argv)
{
    std::cout << "First camera look" << std::endl;
    //std::string serial = "2020018";
    cv::String window_name = "Intensity";
    std::vector<tof::FrameType> types = {tof::FrameType::INTENSITY};

    try
    {

        // serial.c_str()
        tof::EmbeddedKeaCamera cam(tof::ProcessingConfig{});
        std::cout << "Connected with camera" << std::endl;
        /*auto config = cam.get_camera_config();

        config.reset();
        config.set_modulation_frequency(0, 50.0);
        config.set_integration_time(0, {300, 300, 300, 300});
        config.set_phase_shifts(0, {0.0, 0.25, 0.5, 0.75});

        cam.set_camera_config(config);
        std::cout << "Uploaded new configuration " << std::endl;*/
        size_t nstreams = tof::select_streams(cam, types);
        if (nstreams != 1)
        {
            std::cout << "WARNING: there are " << nstreams << " for intensity!" << std::endl;
        }

        //cam.get_calibration();
        //cam.set_calibration();

        cv::namedWindow(window_name, cv::WINDOW_NORMAL);

        cam.start();
        std::cout << "Camera is started" << std::endl;
        while (cam.is_streaming())
        {
            auto frames = cam.get_frames();
            tof::Data &frame = frames.at(0);
            std::cout << "Frame count: " << frame.frame_count() << std::endl;

            cv::Mat img(frame.rows(), frame.cols(), CV_8UC1, frame.data());
            // Now display the image
            cv::imshow(window_name, img);

            // If escape is selected then close
            if (cv::waitKey(10) == 27)
            {
                cam.stop();
            }
        }
        std::cout << "Camera not streaming" << std::endl;
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