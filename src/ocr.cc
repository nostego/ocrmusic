#include "ocr.hh"

Ocr::Ocr(cv::Mat* img, std::vector<Symbol>* vsym)
  : img_(img),
    vsym_(vsym)
{
}

void Ocr::findGKeys()
{
}

void Ocr::findFKeys()
{
}
