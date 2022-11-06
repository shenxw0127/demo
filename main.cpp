#include <NetworkManager.h>
#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
bool reg = 0;
float trans_x(float x)
{
    return ((x-240)/12)/1.1;
}
float trans_y(float y){
    return(((y-480)/480)*33);
}
void error_handle(int error_id, std::string message);
Net::NetworkManager net("192.168.1.2",666, "test", 25562, 25564, error_handle);
void sigint_handler(int sig) { exit(1); }
int main() {
    signal(SIGINT, sigint_handler);
    reg = net.registerUser(0);

    while (!reg) {
        std::cout << "Register failed, retrying..." << std::endl;
        reg = net.registerUser(0);
    }
    std::cout << "Register success" << std::endl;
    float yaw =180;
    float pitch = 0;
    int flag=0;
    int shoot=0;
    net.sendPulse();
    while (true) {
        while (!reg) {
            std::cout << "Register failed, retrying..." << std::endl;
            reg = net.registerUser(0);
        }
        int point_num=0;
        cv::Mat img;
        img = net.getNewestRecvMessage().img;
        float speed=net.getNewestRecvMessage().bullet_speed;
       int time=net.getNewestRecvMessage().buff_over_time;
       int rest=net.getNewestRecvMessage().rest_time;
        if (!img.empty()) {
            bool dafu=false;

            if ((rest>105||time!=0)&&time<25){
                yaw=-3.9;pitch=-11.9;
                Mat channels[3];
                Point2f dafu_center;
                                split(img,channels);
                                Mat red=channels[2]-channels[0];
                                cv::threshold(red,red,44,255,cv::THRESH_BINARY_INV);
                                cv::threshold(red,red,31,255,cv::THRESH_BINARY_INV);
                                Rect rect=Rect(Point(230,123),Point(440,335));
                                Mat ROI=red(rect);
                                vector<vector<Point>> contours1;
                                vector<Vec4i> hierarchy1;
                                Mat test=img.clone();
                                GaussianBlur(ROI,ROI,Size(5,5),15,0);
                                findContours(ROI, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
                                for (int t = 0; t < contours1.size(); t++)
                                {
                                    RotatedRect min= minAreaRect(contours1[t]);
                                    Point2f points[4];
                                    min.points(points);
                                      // cout<<min.size.height<<"  "<<min.size.width<<endl;
                                  if(min.size.height/min.size.width>1.5||min.size.height/min.size.width<2.5||min.size.height/min.size.width<0.66||min.size.height/min.size.width>0.4) {
                                      if(min.size.height*min.size.width>800){dafu_center=min.center;
                                                     //     cout<<dafu_center.x<<"   "<<dafu_center.y<<endl;
                                      for (int i = 0; i < 4; i++) {
                                          line(test, points[i], points[(i + 1) % 4], Scalar(0, 0, 255), 2);
                                      }break;
                                      }
                                  }
                                }
                                cv::imshow("img2", img);
                                cv::imshow("img", test);
                                cv::imshow("red", ROI);
                                cv::waitKey(1);
                                cout<<time<<endl;
                               if(dafu_center.x>140&&dafu_center.x<170)
                                {
                                   waitKey(100+(int)speed);net.sendControlMessage(Net::SendStruct(yaw+0.2,(pitch+speed-18.45), 1, 0, -1, 0, 0.0,0.0, -1, -1));
                                                   waitKey(185);
                                                   cout<<speed<<endl;
                               }else{
                               net.sendControlMessage(Net::SendStruct(yaw, pitch, 0, 0, -1, 0, 0.0,0.0, -1, -1));}

            }
            else{
                Mat channels[3];//识别起点
                split(img,channels);//BGR通道分离
                Mat red,blue;
                subtract(channels[2],channels[0],red);//RED-BLUE只显示红色
                Mat fushi = getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
                dilate(red,red,fushi);
                Mat two_red;
                threshold(red,two_red,55, 255,THRESH_BINARY_INV);
                vector<vector<Point>> contours;
                vector<Vec4i> hierarchy;
                findContours(two_red, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
                Mat contour_img=img.clone();Point2f center_point[contours.size()];int t;
                for (t = 0; t < contours.size(); t++) {
                    RotatedRect rect = minAreaRect(contours[t]);
                    if(rect.size.height*rect.size.width>150&&rect.size.height*rect.size.width<10000&&(rect.size.height/rect.size.width)>0.85&&(rect.size.height/rect.size.width)<1.15)   {
                        Point2f P[t][4];
                        rect.points(P[t]);
                        point_num++;
                    }//确定point_num
                }
                Point2f Tect[point_num][4];point_num=0;
                for (t = 0; t < contours.size(); t++)
                {
                    RotatedRect rect = minAreaRect(contours[t]);
                    if(rect.size.height*rect.size.width>150&&rect.size.height*rect.size.width<10000&&(rect.size.height/rect.size.width)>0.85&&(rect.size.height/rect.size.width)<1.15)   {
                        rect.points(Tect[point_num]);
                        center_point[point_num]=Point2f((Tect[point_num][0].x + Tect[point_num][1].x + Tect[point_num][2].x + Tect[point_num][3].x) / 4, (Tect[point_num][0].y + Tect[point_num][1].y + Tect[point_num][2].y + Tect[point_num][3].y) / 4);
                        circle(contour_img,center_point[point_num],5,Scalar(0,0,255),1,LINE_8,0);
                        point_num++;
                    }
                }//图像识别中点
                if (point_num==4)
                {
                    /* for (int temp1 = 0; temp1 < point_num; temp1++) {
                         for (int j = 0; j <= 3; j++) {
                             line(contour_img, Tect[temp1][j], Tect[temp1][(j + 1) % 4], Scalar(0, 0, 255), 2);
                         }
                     }*/ // 绘制center_point
                    Point2f center=Point2f ((center_point[0].x+center_point[1].x+center_point[2].x+center_point[3].x)/4,(center_point[0].y+center_point[1].y+center_point[2].y+center_point[3].y)/4);
                    //  circle(contour_img,center,5,Scalar(0,0,255),1,LINE_8,0);
                    yaw+= trans_x(center.x);
                    pitch += trans_y(center.y)-3.5-(30/speed);
                    shoot=1;
                    if(yaw<20||yaw>340){yaw=180;}
                    net.sendControlMessage(Net::SendStruct(yaw-2.1, pitch-1.8, shoot, 0, -1, 0, 0.0,0.0, 480, 960));
                    waitKey(100);
                    net.sendControlMessage(Net::SendStruct(yaw+2.2, pitch-1.8, shoot, 0, -1, 0, 0.0,0.0, 480, 960));
                    waitKey(100);
                }
                else
                {
                    pitch=-10;
                    if(yaw>310){flag=1;}
                    if(yaw<50){flag=0;}
                    if(flag==0){yaw+=1;}
                    if(flag==1){yaw-=1;}
                    shoot=0;
                    net.sendControlMessage(Net::SendStruct(yaw, pitch, shoot, 0, -1, 0, 0.0,0.0, 480, 960));
                }
                //  imshow("blue",blue);
                //    imshow("cou",contour_img);
                //  imshow("1",channels[1]);
                // imshow("red",two_red);
                //    cv::imshow("img", img);
                cv::waitKey(1);
            }}
        //   net.sendControlMessage(Net::SendStruct(yaw, pitch, shoot, 0, -1, 0, 0.0,0.0, 480, 960));

    }
    return 0;
}

void error_handle(int error_id, std::string message) {
    if (error_id == 1)
    {
        reg = 0;
    }
    std::cout << "Error: " << error_id << " " << message << std::endl;
}