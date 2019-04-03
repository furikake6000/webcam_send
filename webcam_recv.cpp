#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "opencv2/opencv.hpp"
// #include "openpose/flags.hpp"
// #include "openpose/core/common.hpp"
// #include "openpose/headers.hpp"

#define WIDTH 32
#define HEIGHT 24
#define PORT 3000

#define TMP_BUF_SIZE 1024

using namespace std;

int main(){
    char imgbuf[WIDTH * HEIGHT * 3];
    int imgbuf_head;
    cv::Mat received_frame;

    char tmpbuf[TMP_BUF_SIZE]; // temporary buffer

    int s;
    struct sockaddr_in addr;
 
    socklen_t sin_size;
    struct sockaddr_in from_addr;

    // udp socket
    if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // addr setting
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    cout << "Start connection..." << endl;

    while(1){

        imgbuf_head = 0;
        while(imgbuf_head < WIDTH * HEIGHT * 3){
            int remainsize = min(WIDTH * HEIGHT * 3 - imgbuf_head, TMP_BUF_SIZE);

            // receive data
            int receivedsize = recvfrom(s, tmpbuf, remainsize, 0, (struct sockaddr *)&from_addr, &sin_size);
            
            // search __frame__ signal
            char* frame_end_sig = strstr(tmpbuf, "_frame_");
            if (frame_end_sig != NULL) {
                // set head to 0
                imgbuf_head = 0;
                receivedsize = frame_end_sig - tmpbuf;
                memcpy(&imgbuf[imgbuf_head], frame_end_sig, receivedsize);
            }else{
                // copy to buffer
                memcpy(&imgbuf[imgbuf_head], tmpbuf, receivedsize);
            }

            imgbuf_head += receivedsize;
        }

        cout << "data: " << imgbuf_head << "bytes " << (int)imgbuf[0] << ", " << (int)imgbuf[1] << ", " << (int)imgbuf[2] << endl;

        received_frame = cv::Mat(cv::Size(WIDTH, HEIGHT), CV_8UC3, imgbuf);

        cv::imshow("camera capture", received_frame);

        int k = cv::waitKey(1);
        if (k == 113){
            break;
        }
    }
}
