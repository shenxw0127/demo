#include <NetworkManager.h>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
bool reg = 0;
void error_handle(int error_id, std::string message);
void juxing(Mat a,Mat b) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    long double x, y;
    findContours(a, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
    for (int t = 0; t < contours.size(); t++) {
        RotatedRect rect = minAreaRect(contours[t]);
        Point2f P[4];
        rect.points(P);

                for (int j = 0; j <= 3; j++) {
                    line(b, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 2);
                }


    }
}
Net::NetworkManager net("127.0.0.1", "嘉然今天吃什么", 25562, 25564, error_handle);
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
            red =channels[2]-channels[0];
            subtract(red,channels[1],red);
            subtract(channels[0],channels[2],blue);
            Mat element = getStructuringElement(MORPH_RECT, Size(1, 1));
            //threshold(red,red,5, 255, THRESH_BINARY_INV);
           // dilate(red,red,element);
            //dilate(red,red,element);
            //dilate(red,red,element);
            //GaussianBlur(blue,blue,Size(9,9),15,0);
           // juxing(blue,img);

            int nengliangjiguan=1;
            if(nengliangjiguan==1){
                pitch=0;

            }
            //------------展示-----------------
            namedWindow("Red",0);
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
