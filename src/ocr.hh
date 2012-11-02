#ifndef OCR_HH
# define OCR_HH

# include <opencv/cv.h>
# include <opencv/highgui.h>
# include "symbol_detection.hh"
# include "tools.hh"

class Ocr
{
  public:
    Ocr(cv::Mat* img, std::vector<Symbol>* vsym);
    void findGKeys();
    void findFKeys();
    std::vector<Symbol> findCandidates();
  private:
    cv::Mat* img_;
    std::vector<Symbol>* vsym_;
};

#endif
