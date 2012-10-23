#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include "line_detection.hh"
#include "preprocess.hh"
#include "tools.hh"

using namespace cv;

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
    std::vector<Line> lines;
    cv::Mat img_display = cv::imread(argv[1]);
    cv::Mat img_process = preprocess(img_display);

    lines = detect_lines(img_process);
    display_lines(img_display, lines);

    display(img_display, 600);
    imwrite("output.png", img_display);
  }
  return 0;
}
