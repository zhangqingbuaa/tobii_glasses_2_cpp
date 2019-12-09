// tobii_example.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include "data_stream.h"
#include "video_stream.h"
#include "eye_stream.h"

#include <string>

using namespace cv;
using namespace std;

int main()
{
	
	printf("This is a c++ example project for tobii glasses2!");
	//std::thread t1(eye_stream_main);
//	std::thread t2(video_stream_main);
	std::thread t3(data_stream_main);
	//t1.join();
//	t2.join();
	t3.join();
	
    return 0;
}

