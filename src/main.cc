#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include "line_detection.hh"
#include "symbol_detection.hh"
#include "note_detection.hh"
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
    cv::Mat img = cv::imread(argv[1]);
    double max_rot = 60.0;
    std::vector<Line> lines;
    std::vector<Symbol> symbols;
    std::vector<Symbol> keys;
    std::vector<cv::Rect> pistes;
    std::vector<Note> notes;

    if ((argc >= 3) && (strcmp(argv[2], "--straight") == 0))
      max_rot = 0.001;

    lines = detect_lines(img, max_rot);
    if (lines.size() > 4)
    {
      detect_symbols(img, lines, symbols, pistes);
      dispatch_keys(pistes, symbols, keys);
      detect_notes(img, lines, pistes, symbols, notes);
      Ocr ocr (&img, keys, pistes);
    }
    display(img, 700);
    //imwrite("output.png", img);
  }
  return 0;
}
