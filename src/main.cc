#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include "line_detection.hh"
#include "symbol_detection.hh"
#include "tools.hh"
#include "ocr.hh"

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
    cv::Mat img = cv::imread(argv[1]);
    double max_rot = 60.0;

    if ((argc >= 3) && (strcmp(argv[2], "--straight") == 0))
      max_rot = 0.001;

    lines = detect_lines(img, max_rot);
    std::vector<Symbol> symbols = detect_symbols(img, lines);
    Ocr ocr (&img, symbols);
    std::cout << "Number of symbols = " << symbols.size() << std::endl;
    //display(img, 700);
    //imwrite("output.png", img);
  }
  return 0;
}
