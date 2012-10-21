#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>
#include <cmath>

#define EPSILON 0.00001
#define MAX_ROT 1

using namespace cv;

struct Line
{
  size_t y;
  size_t h;
  double angle;
};

void display(cv::Mat& img,
             double target_display_w);

bool sort_line(Vec4i i, Vec4i j)
{
  return (std::min(i[1], i[3]) < std::min(j[1], j[3]));
}

bool unique_line(Vec4i i, Vec4i j)
{
  return (std::min(i[1], i[3]) == std::min(j[1], j[3]));
}

std::vector<Vec4i> detect_lines(cv::Mat& img)
{
  std::vector<Vec4i> lines;
  std::vector<Vec4i> ret;
  std::vector<Line> mylines;

  HoughLinesP(img, lines, 1, M_PI / 2.0, 70.0, 10.0);
  for (size_t i = 0; i < lines.size(); ++i)
  {
    if ((sqrt(pow(lines[i][0] - lines[i][2], 2.0) +
              pow(lines[i][1] - lines[i][3], 2.0)) > img.size().width * 0.1) &&
        ((180.0 / M_PI) * fabs(atan2(lines[i][3] - lines[i][1], lines[i][2] - lines[i][0])) <= MAX_ROT))
    {
      ret.push_back(lines[i]);
    }
  }

  int lasty = -1;

  // Merge lines
  sort(ret.begin(), ret.end(), sort_line);
  for (size_t i = 0; i < ret.size(); ++i)
  {
    bool add = true;

    for (size_t j = i + 1; j < ret.size(); ++j)
      if (ret[i][1] == ret[j][1])
        add = false;

    if (add)
    {
      if ((lasty < 0) ||
          (fabs(std::min(ret[i][1], ret[i][3]) - lasty) > 1))
      {
        Line n;

        n.angle = atan2(ret[i][3] - ret[i][1], ret[i][2] - ret[i][0]);
        n.y = ret[i][1] - sin(n.angle) * ret[i][0];
        n.h = 1;
        mylines.push_back(n);
      }
      else
      {
        ++mylines[mylines.size() - 1].h;
      }
      lasty = std::min(ret[i][1], ret[i][3]);
    }
  }

  // 5 lines pattern
  double dist[4];

  for (size_t i = 0; i < mylines.size() - 5;)
  {
    double mean = 0.0;
    bool ok = true;

    for (size_t k = 0; k < 4; ++k)
    {
      dist[k] = fabs((double)mylines[i + k].y -
                     (double)mylines[i + k + 1].y);
      mean += dist[k];
    }
    mean /= 4.0;

    for (size_t k = 0; k < 4; ++k)
    {
      dist[k] -= mean;
      if (dist[k] > 1.0)
        ok = false;
    }
    if (ok)
      i += 5;
    else
    {
      mylines.erase(mylines.begin() + i);
    }
  }

  ret.clear();
  for (size_t i = 0; i < mylines.size(); ++i)
  {
    for (size_t hi = 0; hi < mylines[i].h; ++hi)
    {
      Vec4i l;

      l[0] = 0;
      l[1] = mylines[i].y + hi;
      l[2] = img.size().width;
      l[3] = mylines[i].y + hi;
      ret.push_back(l);
    }
  }

  return ret;
}

//TO MODIFY
void remove_lines(cv::Mat& img,
                  std::vector<Vec4i>& lines)
{
  Scalar sc;

  sc[0] = 0;
  cv::Mat mask(img.size(), CV_8UC1, sc);

  for( size_t i = 0; i < lines.size(); i++ )
    line(mask, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(255), 1, 8);

  for (int y = 0; y < img.size().height; ++y)
    for (int x = 0; x < img.size().width; ++x)
      if (mask.at<uchar>(y, x) > 0)
      {
        bool something = 0;

        if ((y >= 1) && (y + 1 < img.size().height))
        {
          something |= (img.at<uchar>(y - 1, x) > 0) && (img.at<uchar>(y + 1, x) > 0);
          if ((x >= 1) && (x + 1 < img.size().width))
          {
            something |= (img.at<uchar>(y - 1, x - 1) > 0) && (img.at<uchar>(y + 1, x + 1) > 0);
            something |= (img.at<uchar>(y - 1, x + 1) > 0) && (img.at<uchar>(y + 1, x - 1) > 0);
          }
        }
        if (!something)
          img.at<uchar>(y, x) = 0;
      }
}

void display_lines(cv::Mat& img,
                   std::vector<Vec4i>& lines)
{
  for( size_t i = 0; i < lines.size(); i++ )
  {
    line(img, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
  }
}

void display(cv::Mat& img,
             double target_display_w)
{
  Mat display(Size(target_display_w,
                   (target_display_w / (double)img.size().width)* img.size().height),
              CV_8UC1);
  resize(img, display, display.size(), 0, 0, INTER_LINEAR);
  namedWindow("Skin");
  imshow("Skin", display);
  waitKey(0);
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: "
              << argv[0]
              << " sheet_image"
              << std::endl;
  }
  else
  {
    cv::Mat img = cv::imread(argv[1]);
    double target_width = img.size().width;
    double target_height = (target_width / (double)img.size().width)* img.size().height;
    Mat dst(Size(target_width, target_height), CV_8UC1);
    Mat dst2(Size(target_width, target_height), CV_8UC1);
    std::vector<Vec4i> lines;

    resize(img, dst, dst.size(), 0, 0, INTER_LINEAR);
    cvtColor(dst, dst2, CV_RGB2GRAY);
    dst = dst2;

    threshold(dst, dst, 200.0, 255.0, THRESH_BINARY_INV);
    lines = detect_lines(dst);
    std::cout << "Total lines = " << lines.size() << std::endl;
    display_lines(img, lines);

    display(img, 600);
    imwrite("output.png", img);

    //remove_lines(dst, lines);
    //imwrite("output_no_line.png", dst);
    //display(dst, 600);
  }
  return 0;
}
