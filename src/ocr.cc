#include "ocr.hh"

Ocr::Ocr(cv::Mat* img, std::vector<Symbol> vsym)
  : img_(img),
    vsym_(vsym)
{
  findKeys(vsym);
}

void Ocr::findKeys(std::vector<Symbol>& choosen)
{
  int diff = 20;
  for (size_t i = 0; i < choosen.size(); ++i)
  {
    if (abs(choosen[i].rect.width - choosen[i].rect.height) > diff)
      display_onerect(*img_, choosen[i].rect, 0x00ff00);
    else
      display_onerect(*img_, choosen[i].rect, 0x00ffff);
  }
}

bool comp(Symbol a, Symbol b)
{
  if (a.rect.x <= b.rect.x)
    return true;
  return false;
}
