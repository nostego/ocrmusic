#ifndef SYMBOL_DETECTION_HH_
# define SYMBOL_DETECTION_HH_

# include "line_detection.hh"
# include <algorithm>

struct Symbol
{
  int pos;
  cv::Rect rect;
};

void remove_pistes(cv::Mat& img,
                   std::vector<cv::Rect>& pistes_rect);

void dispatch_keys(std::vector<cv::Rect>& pistes,
		   std::vector<Symbol>& symbols,
		   std::vector<Symbol>& keys);
void symboldetection_preprocess(cv::Mat& img,
                                cv::Mat& ret,
                                std::vector<Line>& lines);
void detect_symbols(cv::Mat& img,
		    std::vector<Line>& lines,
		    std::vector<Symbol>& symbols,
		    std::vector<cv::Rect>& pistes_rect);
#endif // !SYMBOL_DETECTION_HH_
