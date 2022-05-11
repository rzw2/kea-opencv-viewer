"""
Example of using the EmbeddedKeaCamera class to access intensity data from the 
camera using Python. 

For issues and support please contact me, Refael Whyte: r.whyte@chronoptics.com 
"""

# %%
# This is a hacky way to get on camera python working, as OpenMP linking isn't working
import numpy as np
import matplotlib.pyplot as plt

import ctypes
from ctypes.util import find_library
tmp_lib = find_library("gomp")
if tmp_lib != None:
    ctypes.CDLL(tmp_lib, mode=ctypes.RTLD_GLOBAL)
    import chronoptics.tof as tof


def handle_close(evt):
    cam.stop()
    return


proc = tof.ProcessingConfig()

user = tof.UserConfig()

cam = tof.EmbeddedKeaCamera(proc)
config = cam.getCameraConfig()

tof.selectStreams(cam, [tof.FrameType.INTENSITY])

fig = plt.figure()
fig.canvas.mpl_connect("close_event", handle_close)

roi = config.getRoi(0)
nrows = roi.getImgRows()
ncols = roi.getImgCols()

plt.subplot(111)
rimg = plt.imshow(
    np.zeros((nrows, ncols), dtype=np.uint8),
    cmap="gray",
    vmin=0,
    vmax=255,
    interpolation="none",
)
plt.title("Radial")

cam.start()
while cam.isStreaming():
    frames = cam.getFrames()

    rimg.set_data(np.asarray(frames[0]))
    plt.draw()

    plt.show(block=False)
    plt.pause(0.1)

# %%
