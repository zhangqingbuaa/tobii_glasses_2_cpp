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
#include <array>
#include <cpr/cpr.h>
#include <json.hpp>

#define IPV6 1
using namespace std;
//using namespace nlohmann;
void keep_alive_timer(string keep_alive_message);
void udp_socket_stream();
nlohmann::json post_request(string api_action);
nlohmann::json post_request(string api_action,string data);
string create_project();
string create_participant(string project_id);
string create_calibration(string project_id, string participant_id);
void start_calibration(string calibration_id);
string create_recording(string participant_id);
void start_recording(string recording_id);
void stop_recording(string recording_id);
string wait_for_status(string api_action, string key, array<string,2> values);
void get_sys_info();
nlohmann::json get_request(string api_action);
//const string SERVER = "192.168.1.100";	//ip address of udp server for udptest
//const int PORT = 10500;	//The port for test
const int BUFLEN = 1024;	//Max length of buffer
bool main_running;
const string KA_DATA_MSG = "{\"type\": \"live.data.unicast\", \"key\": \"some_GUID\", \"op\": \"start\"}";
//const string KA_EYES_MSG = "{\"type\": \"live.eyes.unicast\", \"key\": \"some_GUID\", \"op\": \"start\"}"; // used to sync eyes
//const string KA_VIDEO_MSG = "{\"type\": \"live.video.unicast\", \"key\": \"some_other_GUID\", \"op\": \"start\"}";
#if IPV6//if connect by wire
struct sockaddr_in6 si_other;
int s;
const string SERVER="fe80::76fe:48ff:fe3d:fc6d";	//ip address of udp server for me
const int PORT = 49152;	//The port on which to listen for incoming data
const string baseurl="http://[fe80::76fe:48ff:fe3d:fc6d]";
#else//if connect by wireless
struct sockaddr_in si_other;
int s;
const string SERVER = "192.168.71.50";	//ip address of udp server
const int PORT = 49152;	//The port on which to listen for incoming data
const string baseurl="http://"+SERVER;
#endif

int data_stream_main()
{
	main_running=true;
	printf("TOBII GLASSES 2 example in c++ \n");
	char buf[BUFLEN];
	char message[BUFLEN];
	#if IPV6
	printf(" Create Socket By ipv6 \n");
	int slen = sizeof(si_other);
	s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	//setup address structure
	memset((char *)& si_other, 0, sizeof(si_other));
	si_other.sin6_family = AF_INET6;
	si_other.sin6_port = htons(PORT);
	inet_pton(AF_INET6, SERVER.c_str(), &(si_other.sin6_addr.s6_addr));
	//si_other.sin6_addr.s6_addr = inet_addr(SREVER_V6.c_str());
	#else
	printf(" Create Socket By ipv4 \n");
	int slen = sizeof(si_other);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//setup address structure
	memset((char *)& si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	//si_other.sin_addr.s_addr = inet_addr(SERVER.c_str());
	inet_pton(AF_INET, SERVER.c_str(), &(si_other.sin_addr.s_addr));
	#endif
	printf("Will now connect and show streamed data\n");
	// OutputStream Keep alive thread
	std::thread keep_data_alive_thread(keep_alive_timer, KA_DATA_MSG); //add threads with more messages
	keep_data_alive_thread.detach();
	//std::thread keep_video_alive_thread(keep_alive_timer, KA_VIDEO_MSG); //without sending this video msg,will not receive the video data in udp but can get video by rstp 
	//keep_video_alive_thread.detach();
	//InputStream Read udp socket stream
	std::thread udp_socket_streams(udp_socket_stream);
	udp_socket_streams.join();
	get_sys_info();
	sleep(5);
	get_sys_info();
	printf("start create project\n");
	string project_id=create_project();
	printf("create_project finish!!\n");
	string participant_id =create_participant(project_id);
	printf("create_participant finish!!\n");
	string calibration_id=create_calibration(project_id,participant_id);
	cout<<"Project: " + project_id+", Participant: "+participant_id+", Calibration: "+calibration_id+" "<<endl;
	printf("Press enter to calibrate\n");
	char enter_str=getchar();
	printf("calibration started...\n");
	start_calibration(calibration_id);
	string status = wait_for_status("/api/calibrations/"+calibration_id+"/status","ca_state",{"failed","calibrated"});
	if(status=="failed"){
		printf("Calibration failed, using default calibration instead\n");
	}
	else{
		printf("Calibration successful\n");
	}
	string recording_id = create_recording(participant_id);
	printf("Recording start...\n");
	start_recording(recording_id);
	sleep(10);
	stop_recording(recording_id);
	status = wait_for_status("/api/recordings/" + recording_id + "/status", "rec_state", {"failed", "done"});
	if(status=="failed"){
		printf("Recording failed\n");
	}
	else{
		printf("Recording successful\n");
	}
	main_running=false;
	return 0;
}

/* These functions handles the udp socket connection and stream */

void keep_alive_timer(const string keep_alive_message) {
	/*Send Keep Alive messages every second */
	int slen = sizeof(si_other);
	char message[BUFLEN];
	std::string data_message = keep_alive_message;
	strcpy(message, data_message.c_str());
	//printf("Start sending keep alive messages \n");
	while (main_running)
	{
		//printf("Keep Alive Data Sent \n");
		sendto(s, message, strlen(message), 0, (struct sockaddr *) & si_other, slen);
		sleep(1);//定时1s
		
	}
	close(s);
	printf("close the alive timer\n");
}

void udp_socket_stream() {
	/*
	Read and print Stream from udp socket.
	*/

	socklen_t slen = sizeof(si_other);
	char buf[BUFLEN];

	while (main_running)
	{
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		//try to receive some data, this is a blocking call
		recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) & si_other, &slen);
		//sleep(1);
		puts(buf);
	}
	close(s);
 }

nlohmann::json get_request(string api_action) {
	string url=baseurl+api_action;
	auto response = cpr::Get(cpr::Url{url},cpr::Header{{"Content-Type","application/json"}});
	auto json_data = nlohmann::json::parse(response.text);	
	//std::cout << json_data.dump(4) << std::endl;
	return json_data;
}
nlohmann::json post_request(string api_action) {
	string url=baseurl+api_action;
	auto response = cpr::Post(cpr::Url{url},cpr::Body{"null"},cpr::Header{{"Content-Type","application/json"}});
	auto json_data = nlohmann::json::parse(response.text);	
	//std::cout << json_data.dump(4) << std::endl;
	return json_data;
}
nlohmann::json post_request(string api_action,string data) {
	string url=baseurl+api_action;
	auto response = cpr::Post(cpr::Url{url},cpr::Body{data},cpr::Header{{"Content-Type","application/json"}});
	auto json_data = nlohmann::json::parse(response.text);	
	//std::cout << json_data.dump(4) << std::endl;
	return json_data;
}
string create_project(){
 	auto json_data =post_request("/api/projects");
	string pr_id = json_data["pr_id"].get<std::string>();
 	//std::cout << pr_id << std::endl;
	return pr_id;
 }

string create_participant(string project_id){
	nlohmann::json data;
    data["pa_project"]=project_id;
    string data_str=data.dump();
    auto json_data = post_request("/api/participants", data_str);
	string pa_id = json_data["pa_id"].get<std::string>();
	std::cout <<json_data["pa_created"]<<std::endl;//time
 	//std::cout << pa_id << std::endl;
    return pa_id;
}

string create_calibration(string project_id, string participant_id){
    nlohmann::json data;
	data["ca_project"]=project_id;
	data["ca_type"]="default";
	data["ca_participant"]=participant_id;
	string data_str=data.dump();
    auto json_data = post_request("/api/calibrations", data_str);
	string ca_id=json_data["ca_id"].get<std::string>();
	//std::cout << ca_id << std::endl;
    return ca_id;
}

void start_calibration(string calibration_id){
	string url="/api/calibrations/" + calibration_id + "/start";
    auto json_data=post_request(url);
}

string create_recording(string participant_id){
	nlohmann::json data;
	data["rec_participant"]=participant_id;
	string data_str=data.dump();
    auto json_data = post_request("/api/recordings", data_str);
    string rec_id=json_data["rec_id"].get<std::string>();
	return rec_id;
}
    
void start_recording(string recording_id){
	string url="/api/recordings/" + recording_id + "/start";
    auto json_data= post_request(url);
}

void stop_recording(string recording_id){
	string url="/api/recordings/" + recording_id + "/stop";
	auto json_data=post_request(url);
}
    

void get_sys_info(){
	auto json_data =get_request("/api/system/status");
	std::cout<<json_data.dump(4)<<std::endl;
}
string wait_for_status(string api_action, string key, array<string,2> values){
	string url=baseurl+api_action;
	string data_key_str;
    bool running = true;
    while(running)
	{
    	auto response = cpr::Get(cpr::Url{url},cpr::Header{{"Content-Type","application/json"}});
        auto json_data = nlohmann::json::parse(response.text);
		data_key_str=json_data[key].get<std::string>();
		for(int i=0;i<2;i++)
		{
			if (data_key_str==values.at(i))
			{
				running = false;
			}
		}
        sleep(1);
	}
    return data_key_str;
}
