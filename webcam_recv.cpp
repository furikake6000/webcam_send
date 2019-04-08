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

#define WIDTH 320
#define HEIGHT 240
#define PORT 3000

#define TMP_BUF_SIZE 1024

using namespace std;

char* memnstr(char* mem, const char* target, int memlen, int targetlen)
{
	if(targetlen == 0) return mem; // if target is null, return mem

    int i, j;
    for(i=0; i<=(memlen - targetlen); i++){
        for(j=0; j<targetlen; j++){
            if(mem[i + j] != target[j]) break;
        }
        if(j == targetlen){
            // target found
            return &mem[i];
        }
    }

    // Not found
    return NULL;
}

cv::Mat receive_frame(int conn){
    static char imgbuf[WIDTH * HEIGHT * 3 + TMP_BUF_SIZE];
    static char tmpbuf[TMP_BUF_SIZE];
    static char* frame_end_sig = NULL;
    static int tmpbuf_size;

    int imgbuf_head = 0;

    // Copy if data remains in tmpbuf
    if (frame_end_sig != NULL){
        // Copy data after frame_end_sig
        char* frame_start_sig = frame_end_sig + 7;

        memcpy(imgbuf, frame_start_sig, &tmpbuf[tmpbuf_size] - frame_start_sig);
        imgbuf_head = tmpbuf + tmpbuf_size - frame_start_sig;
    }

    // Data receive
    while (1) {
        if (imgbuf_head >= WIDTH * HEIGHT * 3) break;

        // receive data
        tmpbuf_size = recv(conn, tmpbuf, TMP_BUF_SIZE, 0);
        // check for endframe signal
        frame_end_sig = memnstr(tmpbuf, "_frame_", tmpbuf_size, 7);
        if (frame_end_sig != NULL) {
            // end of frame
            // Copy data before endframesig
            memcpy(&imgbuf[imgbuf_head], tmpbuf, (frame_end_sig - tmpbuf));
            break;
        }else{
            // copy to buffer
            memcpy(&imgbuf[imgbuf_head], tmpbuf, tmpbuf_size);
            // move head index
            imgbuf_head += tmpbuf_size;
        }
    }

    // Make mat
    return cv::Mat(cv::Size(WIDTH, HEIGHT), CV_8UC3, imgbuf);
}

int main(){
    char imgbuf[WIDTH * HEIGHT * 3 + TMP_BUF_SIZE];
    int imgbuf_head = 0;
    cv::Mat received_frame;

    char tmpbuf[TMP_BUF_SIZE + 1]; // temporary buffer
    int tmpbuf_size;
    char* frame_end_sig = NULL;

    int s, conn;
    struct sockaddr_in addr;
 
    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in from_addr;

    // tcp socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    // listen
    if(listen(s, 1) < 0){
        perror("listen");
        return -1;
    }

    cout << "Waiting for connection..." << endl;

    // wait for connection
    if((conn = accept(s, (struct sockaddr *)&from_addr, &sin_size)) < 0) {
        perror("accept");
        return -1;
    }

    cout << "Connection established." << endl;

    while(1){

        received_frame = receive_frame(conn);

        cv::imshow("camera capture", received_frame);

        int k = cv::waitKey(1);
        if (k == 113){
            break;
        }
    }
}
