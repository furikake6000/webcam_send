import sys
ros_cv_path = '/opt/ros/kinetic/lib/python2.7/dist-packages'
if(ros_cv_path in sys.path):
    sys.path.remove(ros_cv_path)
import numpy as np
import cv2
import threading

from socket import socket, AF_INET, SOCK_DGRAM

WIDTH = 32
HEIGHT = 24

ADDR = 'localhost'
if(len(sys.argv) > 1):
    ADDR = sys.argv[1]

def capture_camera(sock, addr, mirror=True):
    """Capture video from camera"""
    # カメラをキャプチャする
    cap = cv2.VideoCapture(0) # 0はカメラのデバイス番号

    try:
        while True:
            _, frame = cap.read()

            # 鏡のように映るか否か
            if mirror is True:
                frame = frame[:,::-1]

            # フレームをリサイズ
            frame = cv2.resize(frame, (WIDTH, HEIGHT))

            # フレームを表示する
            # cv2.imshow('camera capture', frame)

            # encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 50]
            # _, encoded = cv2.imencode('.jpg', frame, encode_param)
            
            # print(encoded.tobytes())

            data = frame.tobytes()
            while len(data) > 0:
                data_part = data[:2048]
                data = data[2048:]
                sock.sendto(data_part, addr)
            
            sock.sendto(b'_frame_', addr)

    except KeyboardInterrupt:
        # キャプチャを解放する
        cap.release()
        cv2.destroyAllWindows()
        sock.close()

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_DGRAM)
    to_addr = (ADDR, 3000)
    # connect to server
    #s.connect((ADDR, 3000))

    capture_camera(s, to_addr)
    
