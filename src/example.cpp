#include <iostream>
#include <typeinfo>
#include <cpr/cpr.h>
#include <json.hpp>
#include <sys/socket.h>
#include <string>
#include <thread>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
using namespace nlohmann;
using namespace std;
struct sockaddr_in6 si_other;
int s;
const string SERVER="fe80::76fe:48ff:fe3d:fc6d";	//ip address of udp server for me
const int PORT = 13006;	//The port on which to listen for incoming data
// int main(int argc, char** argv) {
//     //cpr::Body data = {{"pa_project","project_id"}};
//     nlohmann::json data;
//     string name ="project_id";
//     cout<<name<<endl;
//     data["pa_project"]=name;
//     std::string data1="null";
//     string data2=data["pa_project"].dump();
//     string data3=data["pa_project"].get<std::string>();
//     cout<<data<<endl;
//     cout<<data2<<endl;
//     cout<<data["pa_project"]<<endl;
//     cout<<data3<<endl;
//     string url = "http://httpbin.org/get";
//     auto response = cpr::Get(cpr::Url{url},cpr::Header{{"Content-Type", "application/json"}});
//     nlohmann::json json = nlohmann::json::parse(response.text);
//     //std::string header1=json["headers"].dump();//将json转换成字符串
//     std::string header1=json["url"].dump();//将json转换成字符串
//     std::cout << json.dump(4) << std::endl;
//     std::cout << json["url"] << std::endl;
//     std::cout << header1 <<std::endl;
//     std::cout << url <<std::endl;
// }
#include <curl/curl.h>

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

int main(int argc, char** argv) {
	printf("TOBII GLASSES 2 example in c++ \n");
	char buf[1024];
	char message[1024];
	printf(" Create Socket By ipv4 \n");
	int slen = sizeof(si_other);
	s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	//setup address structure
	memset((char *)& si_other, 0, sizeof(si_other));
	si_other.sin6_family = AF_INET6;
	si_other.sin6_port = htons(PORT);
	inet_pton(AF_INET6, SERVER.c_str(), &(si_other.sin6_addr.s6_addr));
    auto curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "http://fe80::76fe:48ff:fe3d:fc6d/api/system/status");
        struct curl_slist *headers=NULL;
        headers =curl_slist_append(headers,"Content-Type:application/json");
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        
        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
        curl_easy_perform(curl);
        cout<<response_string.c_str()<<endl;
        curl_easy_cleanup(curl);
        curl = NULL;
}