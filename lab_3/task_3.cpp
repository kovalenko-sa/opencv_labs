#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <array>

using namespace std;
using namespace cv;

void morphoFiltering(Mat &src);

void processRobots(Mat &src, Mat &src_color, const Scalar& color, Point bulbCenter);

Point findLamp(Mat &src_color, Mat &src_hsv, const Scalar& color);

void copeWithBeam (Mat &src, vector<vector<Point>> &contours);

Point center(vector<Point> contour) {
    Rect r = boundingRect(contour);
    Point center(r.x+0.5*r.width, r.y+0.5*r.height);
    return center;
}

int BEAM_WIDTH = 25;
int BULB_AND_LAMPSHADE_DIST = 40;

void findRobots(Mat &src_color) {
    Mat src_hsv = src_color.clone();
    
    cvtColor(src_color, src_hsv, COLOR_BGR2HSV);

    Point bulbCenter = findLamp(src_color, src_hsv, CV_RGB(0, 0, 0));

    Mat threshold_red;
    Mat threshold_red_add;
    Mat threshold_green;
    Mat threshold_blue;
    inRange(src_hsv, Scalar(0, 50, 100), Scalar(10 , 255, 255), threshold_red);
    inRange(src_hsv, Scalar(170, 50, 100), Scalar(180, 255, 255), threshold_red_add);
    inRange(src_hsv, Scalar(68, 50, 100), Scalar(78, 255, 255), threshold_green);
    inRange(src_hsv, Scalar(90, 50, 50), Scalar(160, 255, 255), threshold_blue);
    threshold_red = threshold_red + threshold_red_add;
    
    Mat kernel;

    morphoFiltering(threshold_red);
    morphoFiltering(threshold_green);
    morphoFiltering(threshold_blue);

    processRobots(threshold_green, src_color, CV_RGB(0, 255, 0), bulbCenter);
    processRobots(threshold_blue, src_color, CV_RGB(0, 0, 255), bulbCenter);
    processRobots(threshold_red, src_color, CV_RGB(255, 0, 0), bulbCenter);


    imshow("Result", src_color);
    waitKey(0);
}

void morphoFiltering(Mat &src) {
    Mat kernel;
    for (int j = 0; j < 2; j++) {
        erode(src, src, kernel);
    }
    for (int j = 0; j < 3; j++) {
        dilate(src, src, kernel);
    }
    for (int j = 0; j < 2; j++) {
        erode(src, src, kernel);
    }
    for (int j = 0; j < 2; j++) {
        dilate(src, src, kernel);
    }
}

void processRobots(Mat &src, Mat &src_color, const Scalar& color, Point bulbCenter) {
    Mat thr_clone = src.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<double> contours_areas;
    double biggest_area = 0;
    findContours(thr_clone, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    copeWithBeam(src, contours);
    thr_clone = src;
    findContours(thr_clone, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (color == CV_RGB(255, 0, 0)) {
        for (int i = 0; i < contours.size(); i++) {
            Rect rect = boundingRect(contours.at(i));
            Point center_img = center(contours.at(i));
            double dist = norm(bulbCenter - center_img);
            if (dist > BULB_AND_LAMPSHADE_DIST - 10 and dist < BULB_AND_LAMPSHADE_DIST + 10
                    and bulbCenter.y > center_img.y) {
                contours.erase(contours.begin() + i);
                i--;
            }
        }
    }
 
    for (int i = 0; i < contours.size(); i++) {
        double current_area = contourArea(contours.at(i));
        if (current_area > biggest_area)
            biggest_area = current_area;
        contours_areas.push_back(current_area);
    }
    double smollest_lamp_dist = 999;
    double smollest_lamp_dist_id = 0;
    for (int i = 0; i < contours.size(); i++) {
    
        if (biggest_area / contours_areas.at(i) >= 5)
            continue;
        Rect rect = boundingRect(contours.at(i));
        drawContours(src_color, contours, i, color, 3);
        double dist = norm(bulbCenter - center(contours.at(i)));
        if (dist < smollest_lamp_dist) {
            smollest_lamp_dist = dist;
            smollest_lamp_dist_id = i;
        }
    }
    circle(src_color, center(contours.at(smollest_lamp_dist_id)), 3, CV_RGB(0, 0, 0), FILLED);
}

Point findLamp(Mat &src_color, Mat &src_hsv, const Scalar& color) {
    Mat threshold_lamp;
    inRange(src_hsv, Scalar(0, 0, 245), Scalar(180 , 10, 255), threshold_lamp);
    morphoFiltering(threshold_lamp);
   
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat thr_clone = threshold_lamp.clone();
    findContours(thr_clone, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Point lamp_center;

    double biggest_area = 0;
    int biggest_area_id = 0;
    vector<double> contours_areas;
    for (int i = 0; i < contours.size(); i++) {
        double current_area = contourArea(contours.at(i));
        if (current_area > biggest_area) {
            biggest_area = current_area;
            biggest_area_id = i;
        }
        contours_areas.push_back(current_area);
    }
    drawContours(src_color, contours, biggest_area_id, color, 3);
    lamp_center = center(contours.at(biggest_area_id));
    circle(src_color, lamp_center, 3, color, FILLED);
    return lamp_center;
}

void copeWithBeam (Mat &src, vector<vector<Point>> &contours) {
    for (int i = 0; i < contours.size(); i++) {
        vector<Point> current_contour = contours.at(i);
        Point current_center = center(contours.at(i));
        for (int j = 0; j < contours.size(); j ++) {
            Point center_img = center(contours.at(j));
            double dist = norm(current_center - center_img);
            if (dist > BEAM_WIDTH - 5 and dist < BEAM_WIDTH + 5
                    and current_center.x < center_img.x) {
                Point print_center(current_center.x + (int)dist/2, current_center.y);
                circle(src, print_center, (int)dist - 5, Scalar(255), FILLED);
            }
        }
    }
    contours.clear();
}

int main() {

    string image = "/home/semyon//opencv_study/lab_3/img_zadan/roboti/roi_robotov.jpg";
    Mat tmp = imread(image, IMREAD_COLOR);
    findRobots(tmp);
  
    return 0;
}