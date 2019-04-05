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

char* memnstr(char* s, char* cp, int n)
{
	char *s1, *s2;

	if( *cp == '\0') return s; /* cp　の文字列長が0なら s を返す */ 

	while( *s != '\0'){
		while(*s != '\0' && *s != *cp) {/* 終端でなく、また、文字が見つからない間、繰り返す　*/
			s++;
		}
		if(*s == '\0') return NULL;/* 見つからない */
		s1 = s;
		s2 = cp;
		while ( *s1 == *s2 && *s1 != '\0'){ /* s1とcpの部分文字列が一致するか */
			s1++;
			s2++;
		}
		if( *s2 == '\0'){/* cp の文字列は、全て一致した */
			return s;
		}
		s++; /* 次の位置から、調べ直す */
	}
	return NULL;/* 見つからない */
}

char* memnstr(char* mem, char* target, int memlen, int targetlen)
{
	char *s1, *s2;

	if(targetlen == 0) return mem; // if target is null, return mem

    int i, j;
    for(i=0; i<=(memlen - targetlen); i++){
        for(j=0; j<targetlen; j++){
            if(mem[i + j] != target[j]) break;
        }
        if(j == targetlen - 1){
            // find target
            return &mem[i + j];
        }
    }

    // Not found
    return NULL;
}

int main(){
    char imgbuf[WIDTH * HEIGHT * 3];
    int imgbuf_head = 0;
    cv::Mat received_frame;

    char tmpbuf[TMP_BUF_SIZE]; // temporary buffer
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

        while (1) {
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

        received_frame = cv::Mat(cv::Size(WIDTH, HEIGHT), CV_8UC3, imgbuf);

        // cv::imshow("camera capture", received_frame);

        // Copy data after endframesig
        char* frame_start_sig = frame_end_sig + 7;

        memcpy(imgbuf, frame_start_sig, &tmpbuf[tmpbuf_size] - frame_start_sig);
        imgbuf_head = tmpbuf - frame_start_sig;

        int k = cv::waitKey(1);
        if (k == 113){
            break;
        }
    }
}
