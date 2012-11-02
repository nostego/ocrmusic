#include "ocr.hh"

Ocr::Ocr(cv::Mat* img, std::vector<Symbol>* vsym)
  : img_(img),
    vsym_(vsym)
{
  findCandidates();
}

void Ocr::findGKeys()
{
}

void Ocr::findFKeys()
{
}

bool comp(Symbol a, Symbol b)
{
  if (a.rect.x <= b.rect.x)
    return true;
  return false;
}

std::vector<Symbol> Ocr::findCandidates()
{
  std::vector<Symbol> choosen;
  int limit = 24;
  sort(vsym_->begin(), vsym_->end(), comp);
  for (size_t i = 0; i <  vsym_->size(); ++i)
  {
    if ((*vsym_)[i].rect.x >= (*vsym_)[0].rect.x &&
        (*vsym_)[i].rect.x <= (*vsym_)[0].rect.x + limit)
      display_onerect(*img_, (*vsym_)[i].rect, 0xff00ff);
    choosen.push_back((*vsym_)[i]);
  }
  return choosen;
}
