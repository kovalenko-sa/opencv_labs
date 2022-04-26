#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/core.hpp> 
#include <opencv2/imgcodecs.hpp> 
#include <iostream>
#include <math.h>

using namespace cv;


Mat3b show_tank(Mat3b background, Mat4b tank, Point pos)
{
  double alpha = 1; 
  Mat3b roi = background(Rect(pos.x, pos.y, tank.cols, tank.rows));

  for (int r = 0; r < roi.rows; ++r)
  {
    for (int c = 0; c < roi.cols; ++c)
    {
      const Vec4b& vf = tank(r,c);
      if (vf[3] > 0) 
      {
        Vec3b& vb = roi(r,c);
        for(int i = 0; i <= 2; i++)
        {
          vb[i] = alpha * vf[i] + (1 - alpha) * vb[i];
        }
      }
    }
  }

  return background;
}

int turn_bullet = 0;


Mat3b show_bullet(Mat3b background, Mat4b bullet, Point pos)
{
  double alpha = 1; 
  //spawn bullet
  if(turn_bullet == 0)
  {
    pos.x += 52;
    pos.y -= 7;
  }
  if(turn_bullet == -1 || turn_bullet == 3)
  {
    pos.x += 130;
    pos.y += 50;
  }
  if(turn_bullet == 1 || turn_bullet == -3)
  {
    pos.x -= 7;
    pos.y += 55;
  }
  if(turn_bullet == 2 || turn_bullet == -2)
  {
    pos.x += 55;
    pos.y += 120;
  }

  Mat3b roi = background(Rect(pos.x, pos.y, bullet.cols, bullet.rows));

  for (int r = 0; r < roi.rows; ++r)
  {
    for (int c = 0; c < roi.cols; ++c)
    {
      const Vec4b& vf = bullet(r,c);
      if (vf[3] > 0) 
      {
        Vec3b& vb = roi(r,c);
        for(int i = 0; i <= 2; i++)
        {
          vb[i] = alpha * vf[i] + (1 - alpha) * vb[i];
        }
      }
    }
  }



  return background;
}

  
  
int main( int argc, char** argv ) 
{
  Mat3b background = imread("/home/semyon/opencv_study/lab_1/field.jpg");
  Mat4b tank = imread("/home/semyon/opencv_study/lab_1/tank.png", IMREAD_UNCHANGED);
  Mat4b bullet = imread("/home/semyon/opencv_study/lab_1/shell7.png", IMREAD_UNCHANGED);
  resize(background, background, Size(1000, 1000), INTER_LINEAR);
  resize(tank, tank, Size(150, 150), INTER_LINEAR);
  resize(bullet, bullet, Size(40, 40), INTER_LINEAR);
  cv::rotate(bullet, bullet, cv::ROTATE_90_COUNTERCLOCKWISE);
  
  Point pos;//tank possition
  Point pos_bullet;
  pos_bullet.x = 425;//450 - center of image
  pos_bullet.y = 425;
  int bullet_flight = 0;//if bullet_button is pressed
  int turn = 0;//where tank is looking
  pos.x = 425;
  pos.y = 425;
   
  while (1) {

    if(turn == -4 || turn == 4)
    {
      turn = 0;
    }
    
    Mat3b background_clone = background.clone();
    int Key = waitKey(10);
    if (Key == 27) break;
    if (Key == 81)
    {
      cv::rotate(tank, tank, cv::ROTATE_90_COUNTERCLOCKWISE);
      cv::rotate(bullet, bullet, cv::ROTATE_90_COUNTERCLOCKWISE);
      turn += 1;
    }
    if (Key == 83)
    {
      cv::rotate(tank, tank, cv::ROTATE_90_CLOCKWISE);
      cv::rotate(bullet, bullet, cv::ROTATE_90_CLOCKWISE);
      turn -= 1;
    }
    if (Key == 82)
    {
      if((turn == 0) && ((pos.y - 10) > 50))
      {
        pos.y -= 10;
      }
      if((turn == -1 || turn == 3) && ((pos.x + 10) < 820))
      {
        pos.x += 10;
      }
      if((turn == 1 || turn == -3) && ((pos.x - 10) > 50))
      {   
        pos.x -= 10;
      }
      if((turn == 2 || turn == -2) && ((pos.y + 10) < 820))
      {
        pos.y += 10;
      }
    } 
    
    if ((Key == 32) && (bullet_flight == 0))
    {
      bullet_flight = 1;
      turn_bullet = turn;
      pos_bullet.x = pos.x;
      pos_bullet.y = pos.y; 
    }
    if (bullet_flight == 1)
    {
      if(turn_bullet == 0)
      {
        pos_bullet.y -= 10;
      }
      if(turn_bullet == -1 || turn_bullet == 3)
      {
        pos_bullet.x += 10;
      }
      if(turn_bullet == 1 || turn_bullet == -3)
      {   
        pos_bullet.x -= 10;
      }
      if(turn_bullet == 2 || turn_bullet == -2)
      {
        pos_bullet.y += 10;
      }
      show_bullet(background_clone, bullet, pos_bullet);
    }

    //when stop showing bullet
    if((pos_bullet.x < 50) || (pos_bullet.y < 50))
    {
      bullet_flight = 0;
    }
    if((pos_bullet.x > 820) || (pos_bullet.y > 820))
    {
      bullet_flight = 0;
    }
    imshow("result", show_tank(background_clone, tank, pos));

  }
  
  destroyAllWindows();

  return 0;

}