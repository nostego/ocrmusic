#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include "line_detection.hh"

using namespace cv;

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
    Mat dst(img.size(), CV_8UC1);
    std::vector<Vec4i> lines;

    cvtColor(img, dst, CV_RGB2GRAY);
    img = dst;

    threshold(img, img, 200.0, 255.0, THRESH_BINARY_INV);
    lines = detect_lines(img);
    display_lines(img, lines);

    display(img, 600);
    imwrite("output.png", img);

    //remove_lines(dst, lines);
    //imwrite("output_no_line.png", dst);
    //display(dst, 600);
  }
  return 0;
}
