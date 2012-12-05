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
#include "musicxml.hh"

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
    std::vector<Piste> pistes;
    std::vector<Note> notes;

    if ((argc >= 3) && (strcmp(argv[2], "--straight") == 0))
      max_rot = 0.001;

    lines = detect_lines(img, max_rot);

    if (lines.size() > 4)
    {
      detect_symbols(img, lines, symbols, pistes);
      dispatch_keys(pistes, symbols, keys);

      // FIXME: move it in the right place, add an invisible line.
      // FIXME: quick and dirty.
      double dis = abs(lines[0].y - lines[1].y);
      int cpt = 5;
      bool new_step = false;
      for (int i = 5; i < lines.size(); i += cpt)
      {
        Line l;
        l.y = lines[i - 1].y + dis;
        l.h = lines[i - 1].h;
        l.angle = lines[i - 1].angle;
        lines.insert(lines.begin() + i, l);
        if (!new_step)
        {
          ++cpt;
          new_step = true;
        }
      }

      detect_notes(img, lines, pistes, symbols, notes);
      Ocr ocr (&img, keys, pistes);
    }
    musicxml("test.xml", notes, pistes);
    display(img, 1600);
    imwrite("output.png", img);
  }
  return 0;
}
