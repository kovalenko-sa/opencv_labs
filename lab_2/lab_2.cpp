#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>

#include <iostream>
#include <cmath>

int disparity = 100;


void unsharpMask(cv::Mat& original, cv::Mat& blurred, cv::Mat& output, double k = 1)
{
	output = original - blurred;
	output = k * output + original;
}

int subsctract(cv::Mat& img1, cv::Mat& img2, cv::Mat& result)
{
	double sumDiff = 0;
	for (int u = 0; u < img1.cols; u++)
	{
		for (int v = 0; v < img1.rows; v++)
		{
			uint8_t diff = abs(img1.at<uint8_t>(v, u) - img2.at<uint8_t>(v, u));
			sumDiff += diff;
			result.at<uint8_t>(v, u) = diff;

		}
	}
	sumDiff = sumDiff * 100 / 255;
	sumDiff = sumDiff / img1.size().area();
	return (int)sumDiff;
}


void box_filter(cv::Mat& image, int boxSize)
{
  boxSize = (boxSize - 1) / 2;
  for (int u = boxSize; u < image.cols - boxSize; u++)
  {
    for (int v = boxSize; v < image.rows - boxSize; v++)
    {
      cv::Rect roiRect(u - boxSize, v - boxSize, 2 * boxSize + 1, 2 * boxSize + 1);
      cv::Mat roi = image(roiRect);
      int sum = cv::sum(roi)[0];
      int mean = (int)(sum / (roi.rows * roi.cols));
      image.at<uint8_t>(v, u) = mean;

    }
  }
}

void box_blur(cv::Mat image)
{
  cv::TickMeter tm; 
  cv::Mat diff(image.rows, image.cols, image.type());
  cv::Mat box_output = image.clone();
  cv::Mat blur_output = image.clone();
  tm.start();
  box_filter(box_output, 7);
  tm.stop();
  std::cout << "box filter: " << tm.getTimeMilli() << std::endl;
  tm.start();
  cv::blur(blur_output, blur_output, cv::Size(7,7));
  tm.stop();
  std::cout << "Blur: " << tm.getTimeMilli() << std::endl;
  disparity = subsctract(box_output, blur_output, diff);
  std::cout << "Similarity: " << 100 - disparity << "%" << std::endl;


  cv::imshow("image", image);
  cv::imshow("box_output", box_output);
  cv::imshow("blur_output", blur_output);
  cv::imshow("diff", diff);
}


void gauss_filter(cv::Mat& image, double sigma, int kernelSize)
{
	cv::Mat kernel(kernelSize, kernelSize, CV_32FC1);
	int kernelOffset = (kernelSize - 1) / 2;

	for (int u = -kernelOffset; u <= kernelOffset; u++)
	{
		for (int v = -kernelOffset; v <= kernelOffset; v++)
		{
			kernel.at<float>(v + (kernelSize - 1) / 2, u + (kernelSize - 1) / 2) = pow(M_E, -(v * v + u * u) / (2*sigma));
		}
	}
  cv::Scalar kernel_sum = cv::sum(kernel);
	kernel = kernel / kernel_sum[0];			

 	cv::filter2D(image, image, -1, kernel, cv::Point(kernelOffset, kernelOffset));

}

void logTransform(cv::Mat& img, double c)
{
	
	cv::MatIterator_<uint8_t> it, end;
	for (it = img.begin<uint8_t>(), end = img.end<uint8_t>(); it != end; ++it)
	{
		*it = (uint8_t)(c * log(*it + 1));
	}
}

void gauss(cv::Mat image)
{
  cv::Mat diff(image.rows, image.cols, image.type());
	
  cv::Mat gauss_copy = image.clone();
  cv::Mat box_copy = image.clone();
  cv::Mat gaussblur_copy = image.clone();
  box_filter(box_copy, 3);
  gauss_filter(gauss_copy, 1, 3);
  cv::GaussianBlur(gaussblur_copy, gaussblur_copy, cv::Size(3, 3), 0.8);
  disparity = subsctract(image, gaussblur_copy, diff);
  std::cout << "Similarity: " << 100 - disparity << "%" << std::endl;
  if (disparity < 5) logTransform(diff, 30);
  cv::imshow("gauss_copy", gauss_copy);
  cv::imshow("gaussblur_copy", gaussblur_copy);
  cv::imshow("box_copy", box_copy);
  cv::imshow("diff", diff);
  cv::imshow("image", image);
}


void unsharp(cv::Mat image)
{
  cv::Mat diff(image.rows, image.cols, image.type());
  cv::Mat gauss_copy = image.clone();
  cv::Mat box_copy = image.clone();
  cv::Mat unsharped(image.rows, image.cols, image.type());
	cv::Mat unsharped2(image.rows, image.cols, image.type());
	
  gauss_filter(gauss_copy, 1.4, 7);
  unsharpMask(image, gauss_copy, unsharped, 2);
  box_filter(box_copy, 7);
  unsharpMask(image, box_copy, unsharped2, 2);
  disparity = subsctract(unsharped, unsharped2, diff);
  std::cout << "Similarity: " << 100 - disparity << "%" << std::endl;
  cv::imshow("unsharped", unsharped);
  cv::imshow("unsharped2", unsharped2);
  cv::imshow("diff", diff);
  cv::imshow("image", image);

}

void laplase_filter(cv::Mat image)
{
  float kernelArray[] = {0.0, 1.0, 0.0, 1.0, -4.0, 1.0, 0.0, 1.0, 0.0};
	
	cv::Mat kernel(3, 3, CV_32FC1, kernelArray);

	cv::filter2D(image, image, -1, kernel, cv::Point(-1, -1));
}


void laplase(cv::Mat image)
{
  cv::Mat laplase_copy = image.clone();
  cv::Mat unsharped3 = image.clone();
  laplase_filter(laplase_copy);
  unsharped3 = unsharped3 - 1 * laplase_copy;
  cv::imshow("laplase_copy", laplase_copy);
  cv::imshow("image", image);
  cv::imshow("unsharped3", unsharped3);
  
  
}

int main( int argc, char** argv ) 
{
  
  cv::Mat image;
  image = imread("/home/semyon/opencv_study/lab_2/Lena.png" ,cv::IMREAD_GRAYSCALE);
  cv::resize(image, image, cv::Size(500, 500));
  //box_blur(image);
  //gauss(image);
  //unsharp(image);
  laplase(image);
  cv::waitKey(0);
  cv::destroyAllWindows();
  return 0;
}