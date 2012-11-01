#ifndef SYMBOL_DETECTION_HH_
# define SYMBOL_DETECTION_HH_

# include "line_detection.hh"
# include <algorithm>

struct Symbol
{
  int pos;
  cv::Rect rect;
};

void symboldetection_preprocess(cv::Mat& img,
                                cv::Mat& ret,
                                std::vector<Line>& lines);

std::vector<Symbol> detect_symbols(cv::Mat& img,
                                   std::vector<Line>& lines);

#endif // !SYMBOL_DETECTION_HH_
