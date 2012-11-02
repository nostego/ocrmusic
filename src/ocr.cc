#include "ocr.hh"

Ocr::Ocr(cv::Mat* img, std::vector<Symbol> vsym)
  : img_(img),
    vsym_(vsym)
{
  std::vector<Symbol> choosen = findCandidates();
  findKeys(choosen);
}

void Ocr::findKeys(std::vector<Symbol>& choosen)
{
  int diff = 20;
  for (size_t i = 0; i < choosen.size(); ++i)
  {
    if (abs(choosen[i].rect.width - choosen[i].rect.height) > diff)
      display_onerect(*img_, choosen[i].rect, 0x00ff00);
    else
      display_onerect(*img_, choosen[i].rect, 0xf00000);
  }
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
  sort(vsym_.begin(), vsym_.end(), comp);

  // FIXME: Naive, X can be different !
  for (size_t i = 0; i < vsym_.size(); ++i)
  {
    if (vsym_[i].rect.x >= vsym_[0].rect.x &&
        vsym_[i].rect.x <= vsym_[0].rect.x + limit)
    {
      display_onerect(*img_, vsym_[i].rect, 0xff00ff);
      choosen.push_back(vsym_[i]);
    }
  }
  return choosen;
}
