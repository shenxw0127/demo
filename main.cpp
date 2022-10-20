#include <NetworkManager.h>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
bool reg = 0;
void error_handle(int error_id, std::string message);
Net::NetworkManager net("127.0.0.1", "test", 25562, 25564, error_handle);
void sigint_handler(int sig) { exit(1); }
int main() {
    signal(SIGINT, sigint_handler);
    reg = net.registerUser(0);
    while (!reg) {
        std::cout << "Register failed, retrying..." << std::endl;
        reg = net.registerUser(0);
    }
    std::cout << "Register success" << std::endl;
    float yaw =0;
    float pitch = 0;
    int shoot=0;
    while (true) {
        while (!reg) {
            std::cout << "Register failed, retrying..." << std::endl;
            reg = net.registerUser(0);
        }
        cv::Mat img;
        img = net.getNewestRecvMessage().img;
        if (!img.empty()) {






            Mat origin,channels[3],red,blue;
            origin=img.clone();
            split(origin, channels);
            subtract(channels[2],channels[0],red);
            subtract(channels[0],channels[2],blue);
            Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
            erode(channels[0],channels[0],element);

            int nengliangjiguan=1;
            if(nengliangjiguan==1){
                pitch=4;

            }
            //------------展示-----------------
            imshow("Blue", blue);
            imshow("Green", channels[1]);
            imshow("Red", red);














            cv::imshow("img", img);
            cv::waitKey(1);
        } else {
            std::cout << "Get an empty image" << std::endl;
            cv::waitKey(100);
        }

        /*
        Code Here! Just define yaw ,pitch ,beat ... which in SendStruct's elements  and will be sent to us.
        */
        net.sendControlMessage(Net::SendStruct(yaw, pitch, shoot, 20.0, 0, 0.0, 0.0, -1, -1));
    }
    return 0;
}
void error_handle(int error_id, std::string message) {
    if (error_id == 1) {
        reg = 0;
    }
    std::cout << "Error: " << error_id << " " << message << std::endl;
 }
