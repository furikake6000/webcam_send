#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "opencv2/opencv.hpp"
// #include "openpose/flags.hpp"
// #include "openpose/core/common.hpp"
// #include "openpose/headers.hpp"

#define WIDTH 320
#define HEIGHT 240
#define PORT 3000

using namespace std;

int main(){
    char imgbuf[WIDTH * HEIGHT * 3];
    int imgbuf_head;
    cv::Mat received_frame;

    char tmpbuf[1024]; // temporary buffer

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

    while(1){

        imgbuf_head = 0;
        while(imgbuf_head < WIDTH * HEIGHT * 3){
            int remainsize = min(WIDTH * HEIGHT * 3 - imgbuf_head, 1024);

            recvfrom(s, tmpbuf, remainsize, 0, (struct sockaddr *)&from_addr, &sin_size);
            strncpy(&imgbuf[imgbuf_head], tmpbuf, remainsize);
        }

        received_frame = cv::Mat(HEIGHT, WIDTH, CV_8UC3, imgbuf);

        cv::imshow("camera capture", received_frame);

        int k = cv::waitKey(1);
        if (k == 113){
            break;
        }
    }
}
