#include "ocr.hh"

Ocr::Ocr(cv::Mat* img, std::vector<Symbol> vsym,
	 std::vector<Piste>& pistes)
  : img_(img),
    vsym_(vsym)
{
  findKeys(vsym, pistes);
}

void Ocr::findKeys(std::vector<Symbol>& choosen,
		   std::vector<Piste>& pistes)
{
  int diff = 20;
  for (size_t i = 0; i < choosen.size(); ++i)
  {
    if (choosen[i].rect.height > pistes[choosen[i].piste].height)
    {
      pistes[choosen[i].piste].key = 0;
      display_onerect(*img_, choosen[i].rect, 0x00ff00);
    }
    else
    {
      pistes[choosen[i].piste].key = 1;
      display_onerect(*img_, choosen[i].rect, 0x00ffff);
    }
  }
}

bool comp(Symbol a, Symbol b)
{
  if (a.rect.x <= b.rect.x)
    return true;
  return false;
}
