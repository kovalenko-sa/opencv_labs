#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <iostream>

using namespace cv;

int main()
{
  Mat img_1 = imread("/home/semyon/opencv_study/image1.jpg");
  Mat img_2 = imread("/home/semyon/opencv_study/image2.jpg");
  resize(img_1, img_1, Size(1000, 1000), INTER_LINEAR);
  resize(img_2, img_2, Size(1000, 1000), INTER_LINEAR);
  int i = 0;
  int j = 0;
  Mat final_img;
  final_img.create(img_1.rows, img_1.cols, CV_8UC3);
  Vec3b final_pixel = img_1.at<Vec3b>(i,j);

  double alpha = 0.9;
  double beta = 1 - alpha;

  for (int i = 0; i < img_1.rows; i++)
  {
    for (int j = 0; j < img_1.cols; j++)
    {
      Vec3b pixel_1 = img_1.at<Vec3b>(i,j);
      Vec3b pixel_2 = img_2.at<Vec3b>(i,j);
      for (int k = 0; k <= 2; k++)
      {
        final_pixel[k] = pixel_1[k] * alpha + pixel_2[k] * beta; 
      }
      final_img.at<Vec3b>(Point(j,i)) = final_pixel;
    } 
  }
  imshow("final_img", final_img);
  waitKey(0);

  return 0;
}

