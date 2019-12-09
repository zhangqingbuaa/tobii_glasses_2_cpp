#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <thread>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <gst/gst.h>

#define IPV6 0
using namespace cv;
using namespace std;
#if IPV6
//using namespace nlohmann;
void keep_alive_timer1(string keep_alive_message);
void udp_socket_stream1();
const int BUFLEN1 = 2048;	//Max length of buffer
bool main_running1;
const string KA_VIDEO_MSG = "{\"type\": \"live.video.unicast\", \"key\": \"some_other_GUID\", \"op\": \"start\"}";
struct sockaddr_in6 si_other1;
int s1;
const string SERVER1="fe80::76fe:48ff:fe3d:fc6d";	//ip address of udp server for me
const int PORT1 = 49152;	//The port on which to listen for incoming data

void keep_alive_timer1(const string keep_alive_message) {
	/*Send Keep Alive messages every second */
	int slen = sizeof(si_other1);
	char message[BUFLEN1];
	std::string data_message = keep_alive_message;
	strcpy(message, data_message.c_str());
	//printf("Start sending keep alive messages \n");
	while (main_running1)
	{
		//printf("Keep Alive Data Sent \n");
		sendto(s1, message, strlen(message), 0, (struct sockaddr *) & si_other1, slen);
		sleep(1);//定时1s
		
	}
	close(s1);
	printf("close the alive timer\n");
}

void udp_socket_stream1() {
	/*
	Read and print Stream from udp socket.
	*/

	socklen_t slen = sizeof(si_other1);
	char buf[BUFLEN1];

	while (main_running1)
	{
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN1);
		//try to receive some data, this is a blocking call
		recvfrom(s1, buf, BUFLEN1, 0, (struct sockaddr *) & si_other1, &slen);
		//sleep(1);
		puts(buf);
	}
	close(s1);
 }

#endif
int video_stream_main()
{	
	#if IPV6
	main_running1=true;
	printf("TOBII GLASSES 2 example in c++ \n");
	char buf[BUFLEN1];
	char message[BUFLEN1];
	printf(" Create Socket By ipv6 \n");
	int slen = sizeof(si_other1);
	s1 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	//setup address structure
	memset((char *)& si_other1, 0, sizeof(si_other1));
	si_other1.sin6_family = AF_INET6;
	si_other1.sin6_port = htons(PORT1);
	inet_pton(AF_INET6, SERVER1.c_str(), &(si_other1.sin6_addr.s6_addr));
	printf("Will now connect and show streamed data\n");
	// OutputStream Keep alive thread
	std::thread keep_video_alive_thread(keep_alive_timer1, KA_VIDEO_MSG); //without sending this video msg,will not receive the video data in udp but can get video by rstp 
	keep_video_alive_thread.detach();
	std::thread udp_socket_streams(udp_socket_stream1);
	udp_socket_streams.join();
	#else
	const std::string videoStreamAddress = "rtsp://192.168.71.50:8554/live/scene";
	cv::VideoCapture capture(videoStreamAddress);
	if (!capture.open(videoStreamAddress)) {
		//Error
	}
	cv::namedWindow("scene", CV_WINDOW_AUTOSIZE);
	cv::Mat frame;
	while (true) {
		if (!capture.read(frame)) {
			//Error
		}
		else {
			cv::imshow("scene", frame);

			cv::waitKey(1);
		}
	}
	#endif
}
