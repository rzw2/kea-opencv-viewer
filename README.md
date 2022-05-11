# On Camera Kea Depth Viewer

![Scheme](doc/chronoptics.png)

## Description
Example of running Python and C++ on the Chronoptics Kea depth camera. [Chronoptics](www.chronoptics.com). Chronoptics designs and develops depth sensing solutions, for more information on Kea please contact Chronoptics, or email me r.whyte@chronoptics.com 

The Kea Yocto layer provides the recipe to build this project and add it to the rootfs. Otherwise generate the SDK and build. 

## Build 
To build, you need to source the Yocto SDK to find the dependencies and cross-compile for aarch64
> mkdir build && cd build
> cmake ../   
> make 

To run on the camera 
> DISPLAY=:0 kea_opencv_viewer 

## Python 
Copy the display_camera_stream.py file over to the camera. You will have to update the PYTHONPATH to include the Chronoptics library on the camera. On the camera
> export PYTHONPATH=/usr/lib/python   
> DISPLAY=:0 python3 display_camera_stream.py 

