#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <iostream>
#include <stdio.h>

using namespace cv;

int main()
{
  Mat background;
  background = imread("/home/semyon/opencv_study/lab_1/field.jpg" ,cv::IMREAD_COLOR);
  resize(background, background, Size(1000, 1000), INTER_LINEAR);
  Mat img = background.clone();
  Point figure = Point(0, img.rows/2);
  namedWindow( "image", WINDOW_AUTOSIZE );

  while (1)
  {
    figure = Point(figure.x + 1, (img.rows/2) + sin(((double)figure.x + 1.0)/50)*200);
    circle(img, Point(figure.x,  figure.y), 2, Scalar(0,255,0), FILLED);
    Mat current_img = img.clone();
    rectangle(current_img, Point(figure.x, figure.y - 50), Point(figure.x + 150, figure.y + 50), Scalar(140, 140, 140), -1);
    rectangle(current_img, Point(figure.x + 10, figure.y - 70), Point(figure.x + 60, figure.y - 50), Scalar(0, 0, 0), -1);
    rectangle(current_img, Point(figure.x + 90, figure.y - 70), Point(figure.x + 140, figure.y - 50), Scalar(0, 0, 0), -1);
    rectangle(current_img, Point(figure.x + 10, figure.y + 70), Point(figure.x + 60, figure.y + 50), Scalar(0, 0, 0), -1);
    rectangle(current_img, Point(figure.x + 90, figure.y + 70), Point(figure.x + 140, figure.y + 50), Scalar(0, 0, 0), -1);
    //robot(current_img, position);
    imshow("image", current_img);
    cv::waitKey(5);

    if (figure.x == (img.cols / 2))
    {
      imwrite("robot.png", current_img);
    }
    if (figure.x == (img.cols - 150))
    {
      break;
    }
  }
  waitKey(0);
  destroyAllWindows();

  return 0;
}

