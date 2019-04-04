import sys
ros_cv_path = '/opt/ros/kinetic/lib/python2.7/dist-packages'
if(ros_cv_path in sys.path):
    sys.path.remove(ros_cv_path)
import os
from sys import platform
import cv2
import numpy as np
import cv2
import threading
import struct
import argparse

from socket import socket, AF_INET, SOCK_STREAM

WIDTH = 320
HEIGHT = 240

# Import Openpose (Windows/Ubuntu/OSX)
dir_path = os.path.dirname(os.path.realpath(__file__))
try:
    # Windows Import
    if platform == "win32":
        # Change these variables to point to the correct folder (Release/x64 etc.) 
        sys.path.append(dir_path + '/../../python/openpose/Release');
        os.environ['PATH']  = os.environ['PATH'] + ';' + dir_path + '/../../x64/Release;' +  dir_path + '/../../bin;'
        import pyopenpose as op
    else:
        # Change these variables to point to the correct folder (Release/x64 etc.) 
        sys.path.append('/usr/local/python');
        # If you run `make install` (default path is `/usr/local/python` for Ubuntu), you can also access the OpenPose/python module from there. This will install OpenPose and the python library at your desired installation path. Ensure that this is in your python path in order to use it.
        # sys.path.append('/usr/local/python')
        from openpose import *
except ImportError as e:
    print('Error: OpenPose library could not be found. Did you enable `BUILD_PYTHON` in CMake and have this Python script in the right folder?')
    raise e

# Flags
parser = argparse.ArgumentParser()
parser.add_argument("--image_path", default="../../../examples/media/COCO_val2014_000000000192.jpg", help="Process an image. Read all standard formats (jpg, png, bmp, etc.).")
args = parser.parse_known_args()

# Custom Params (refer to include/openpose/flags.hpp for more parameters)
params = dict()
params["model_folder"] = "/home/furikake/Github/openpose/models"

# Add others in path?
for i in range(0, len(args[1])):
    curr_item = args[1][i]
    if i != len(args[1])-1: next_item = args[1][i+1]
    else: next_item = "1"
    if "--" in curr_item and "--" in next_item:
        key = curr_item.replace('-','')
        if key not in params:  params[key] = "1"
    elif "--" in curr_item and "--" not in next_item:
        key = curr_item.replace('-','')
        if key not in params: params[key] = next_item

WIDTH = 320
HEIGHT = 240

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    # listen to another device
    s.bind(('', 3000))
    s.listen(1)
    conn, addr = s.accept() # Get connection
    print('Connected by', addr)

    # Starting OpenPose
    opWrapper = op.WrapperPython()
    opWrapper.configure(params)
    opWrapper.start()

    lastbufdata = b''
    while True:
        data = lastbufdata
        while True:
            receivedstr=conn.recv(1024*8)
            index = receivedstr.find(b'_frame_')
            if(index != -1):
                data += receivedstr[:index]
                lastbufdata = receivedstr[(index + 7):]
                break
            data += receivedstr
        if not data:
           break

        if len(data) < WIDTH * HEIGHT * 3:
            data += [0] * (WIDTH * HEIGHT * 3 - len(data))
        elif len(data) > WIDTH * HEIGHT * 3:
            data = data[:WIDTH * HEIGHT * 3]

        received_frame = np.frombuffer(data, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

        # Process Image
        datum = op.Datum()
        datum.cvInputData = received_frame
        opWrapper.emplaceAndPop([datum])

        cv2.imshow('camera capture', datum.cvOutputData)
        k = cv2.waitKey(1) # 1msec待つ
        if k == 113: # Qキーで終了
            break

    s.close()
