import numpy as np
import cv2
import threading

from socket import socket, AF_INET, SOCK_STREAM

WIDTH = 640
HEIGHT = 480

def capture_camera(sock, mirror=True):
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
            sock.send(data)
    except KeyboardInterrupt:
        # キャプチャを解放する
        cap.release()
        cv2.destroyAllWindows()
        sock.close()

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    # connect to server
    s.connect(('localhost', 3000))

    capture_camera(s)
    