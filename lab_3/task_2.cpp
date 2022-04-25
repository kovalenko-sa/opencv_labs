#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <array>


using namespace std;
using namespace cv;

const double MIN_CONTOUR_AREA = 80;
const double MAX_TO_MIN_COUNTOUR_RATIO = 2;

Point center(vector<Point> contour) {
    Rect r = boundingRect(contour);
    Point center(r.x+0.5*r.width, r.y+0.5*r.height);
    return center;
}
void aim_search(string img_path) {
    Mat src_color = imread(img_path, IMREAD_COLOR);
    Mat src_hsv = src_color.clone();
    cvtColor(src_color, src_hsv, COLOR_BGR2HSV);
    Mat threshold_color;
    inRange(src_hsv, Scalar(0, 50, 100), Scalar(30, 255, 255), threshold_color);
    Mat kernel;
    for (int j = 0; j < 1; j++) {
        erode(threshold_color, threshold_color, kernel);
    }
    for (int j = 0; j < 3; j++) {
        dilate(threshold_color, threshold_color, kernel);
    }

    Mat thr_clone = threshold_color.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<double> contours_areas;
    double biggest_area = 0;
    findContours(thr_clone, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        double current_area = contourArea(contours.at(i));
        if (current_area > biggest_area)
            biggest_area = current_area;
        contours_areas.push_back(current_area);
    }
    for (int i = 0; i < contours.size(); i++) {
        if (biggest_area / contours_areas.at(i) >= MAX_TO_MIN_COUNTOUR_RATIO)
            continue;
        drawContours(src_color, contours, i, CV_RGB(0, 0, 0), 2);
        circle(src_color, center(contours.at(i)), 5, CV_RGB(0, 0, 0), FILLED);
    }
    imshow("Aim", src_color);
    waitKey(0);
}

int main() {

    string image = "/home/semyon//opencv_study/lab_3/img_zadan/teplovizor/size0-army.mil-2008-08-28-082221.jpg";
    aim_search(image);

    return 0;
}