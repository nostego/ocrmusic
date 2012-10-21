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
              << " sheet_image [--straight]"
              << std::endl;
  }
  else
  {
    std::vector<Line> lines;
    cv::Mat img_display = cv::imread(argv[1]);
    cv::Mat img_process = preprocess(img_display);
    double max_rot = 60.0;

    if ((argc >= 3) && (strcmp(argv[2], "--straight") == 0))
      max_rot = 0.001;

    lines = detect_lines(img_process, max_rot);
    display_lines(img_display, lines);

    //display(img_process, 600);
    display(img_display, 600);
    imwrite("output.png", img_display);
  }
  return 0;
}
