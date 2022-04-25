
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace cv;

Point center(vector<Point> contour) {
    Rect r = boundingRect(contour);
    Point center(r.x+0.5*r.width, r.y+0.5*r.height);
    return center;
}

void aim_search(string img_path) {
    Mat src_color = imread(img_path, IMREAD_COLOR);
    Mat src = imread(img_path, IMREAD_GRAYSCALE);
    Mat thr;
    threshold(src, thr, 200, 255, THRESH_BINARY);
    Mat kernel;
    for (int j = 0; j < 3; j++) {
        erode(thr, thr, kernel);
    }
    for (int j = 0; j < 5; j++) {
        dilate(thr, thr, kernel);
    }

    Mat thr_clone = thr.clone();
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(thr_clone, contours, hierarchy,
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        drawContours(src_color, contours, i, CV_RGB(255, 0, 0), 2);
        circle(src_color, center(contours.at(i)),
               5, CV_RGB(255, 0, 0), FILLED);
    }
    imshow("Aim", src_color);
    waitKey(0);
}

int main() {

    string image = "/home/semyon//opencv_study/lab_3/img_zadan/allababah/ig_2.jpg";
    aim_search(image);

    return 0;
}