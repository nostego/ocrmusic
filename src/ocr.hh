#ifndef OCR_HH
# define OCR_HH

# include <opencv/cv.h>
# include <opencv/highgui.h>
# include "symbol_detection.hh"
# include "tools.hh"

class Ocr
{
public:
  Ocr(cv::Mat* img, std::vector<Symbol> vsym,
      std::vector<Piste>& pistes);
  void findKeys(std::vector<Symbol>& choosen,
		std::vector<Piste>& pistes);
private:
  cv::Mat* img_;
  std::vector<Symbol> vsym_;
};

#endif
