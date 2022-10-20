#include <NetworkManager.h>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>

bool reg = 0;

void error_handle(int error_id, std::string message);

Net::NetworkManager net("175.164.26.59", "test", 25562, 25564, error_handle);

void sigint_handler(int sig) { exit(1); }

int main() {
    signal(SIGINT, sigint_handler);
    reg = net.registerUser(0);
    while (!reg) {
        std::cout << "Register failed, retrying..." << std::endl;
        reg = net.registerUser(0);
    }
    std::cout << "Register success" << std::endl;
    float yaw =100;
    float pitch = 0;
    while (true) {
        while (!reg) {
            std::cout << "Register failed, retrying..." << std::endl;
            reg = net.registerUser(0);
        }
        cv::Mat img;
        img = net.getNewestRecvMessage().img;
        if (!img.empty()) {

            cv::imshow("img", img);
            cv::waitKey(1);
        } else {
            std::cout << "Get an empty image" << std::endl;
            cv::waitKey(100);
        }

        /*
        Code Here! Just define yaw ,pitch ,beat ... which in SendStruct's elements  and will be sent to us. 
        */
        net.sendControlMessage(Net::SendStruct(yaw, pitch, 0, 20.0, 0, 0.0, 0.0, -1, -1));
    }
    return 0;
}

void error_handle(int error_id, std::string message) {
    if (error_id == 1) {
        reg = 0;
    }
    std::cout << "Error: " << error_id << " " << message << std::endl;
 }
