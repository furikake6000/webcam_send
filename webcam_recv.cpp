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
    std::string imgbufstr;
    int imgbufsize;
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
        /*imgbufsize = 0;
        while(imgbufsize < WIDTH * HEIGHT * 3 - 1){
            if(imgbufsize + sizeof(tmpbuf) > WIDTH * HEIGHT * 3 - 1){
                // Finishing frame load
                int remainsize = WIDTH * HEIGHT * 3 - 1 - imgbufsize;
                memset(tmpbuf, 0, sizeof(tmpbuf));
                recvfrom(s, tmpbuf, remainsize, 0, (struct sockaddr *)&from_addr, &sin_size);
            }else{
                recvfrom(s, tmpbuf, sizeof(tmpbuf), 0, (struct sockaddr *)&from_addr, &sin_size);
            }
            
            
        }*/
        imgbufstr = "";
        while(imgbufstr.length() < WIDTH * HEIGHT * 3){
            int remainsize = min(WIDTH * HEIGHT * 3 - (int)imgbufstr.length(), 1024);

            recvfrom(s, tmpbuf, remainsize, 0, (struct sockaddr *)&from_addr, &sin_size);
            string tmpdatastr= tmpbuf;
            tmpdatastr = tmpdatastr.substr(0, remainsize);
            imgbufstr += tmpdatastr;
        }

        printf("Received %d bytes.", (int)imgbufstr.length());

        strncpy(imgbuf, imgbufstr.c_str(), WIDTH * HEIGHT * 3);
        received_frame = cv::Mat(HEIGHT, WIDTH, CV_8UC3, imgbuf);

        cv::imshow("camera capture", received_frame);

        int k = cv::waitKey(1);
        if (k == 113){
            break;
        }
    }
}
