#ifndef SYMBOL_DETECTION_HH_
# define SYMBOL_DETECTION_HH_

# include "line_detection.hh"
# include <algorithm>

struct Symbol
{
  int pos;
  int piste;
  cv::Rect rect;
};

struct Piste : public cv::Rect
{
  int key;
};

void remove_pistes(cv::Mat& img,
                   std::vector<Piste>& pistes_rect);

void dispatch_keys(std::vector<Piste>& pistes,
		   std::vector<Symbol>& symbols,
		   std::vector<Symbol>& keys);
void symboldetection_preprocess(cv::Mat& img,
                                cv::Mat& ret,
                                std::vector<Line>& lines);
void detect_symbols(cv::Mat& img,
		    std::vector<Line>& lines,
		    std::vector<Symbol>& symbols,
		    std::vector<Piste>& pistes_rect);
#endif // !SYMBOL_DETECTION_HH_
