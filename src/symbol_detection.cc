#include "symbol_detection.hh"
#include "tools.hh"
#include "ocr.hh"

void symboldetection_preprocess(cv::Mat& img,
                                cv::Mat& ret,
                                std::vector<Line>& lines)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 195.0, 255.0, cv::THRESH_BINARY_INV);
  (void)lines;
}

std::vector<cv::Rect> get_piste_rect(std::vector<Line>& lines,
                                     cv::Mat& img)
{
  int tmp[img.size().width];
  int drange = img.size().width * 0.1;
  std::vector<cv::Rect> rect;
  double piste_height = fabs(lines[4].y - lines[0].y);
  double interlap = piste_height / 4.0;

  for (size_t k = 0; k < lines.size(); k += 5)
  {
    cv::Rect r;

    r.x = img.size().width;

    for (int lin = 0; lin < 5; ++lin)
    {
      for (int u = 0; u < img.size().width; ++u)
        tmp[u] = 0;

      for (int x = 0; x < img.size().width; ++x)
        for (int range = -drange; range < drange; ++range)
          if (isvalid(img, x + range, lines[k + lin].y))
            tmp[x] += img.at<uchar>(lines[k + lin].y, x + range) / 255;

      int dx = 0;
      while (dx + drange < img.size().width)
      {
        if (tmp[dx + drange] >= drange * 2.0)
          break;
        ++dx;
      }
      r.x = std::min(r.x, dx);
    }
    r.width = img.size().width;
    r.y = lines[k].y - interlap;
    r.height = piste_height + interlap * 2;
    rect.push_back(r);
  }

  return rect;
}



void filter_bbox(std::vector<cv::Rect>& boundRect,
                 std::vector<Line>& lines,
                 std::vector<cv::Rect>& pistes_rect)
{
  bool del[boundRect.size()];
  int epsilon = pistes_rect[0].width * 0.005;

  (void)lines;
  for (size_t i = 0; i < boundRect.size(); ++i)
  {
    int onpiste = 0;

    del[i] = false;
    for (size_t u = 0; u < pistes_rect.size(); ++u)
      onpiste += collide(pistes_rect[u], boundRect[i]);
    if (onpiste != 1)
      del[i] = true;
  }
  filter(boundRect, del);


  for (size_t i = 0; i < boundRect.size(); ++i)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); ++i)
    for (size_t j = i + 1; j < boundRect.size(); ++j)
      if ((boundRect[j].x == boundRect[i].x) &&
          (boundRect[j].y == boundRect[i].y) &&
          (boundRect[j].width == boundRect[i].width) &&
          (boundRect[j].height == boundRect[i].height))
        del[j] = true;
  filter(boundRect, del);

  for (size_t i = 0; i < boundRect.size(); ++i)
    del[i] = false;

  for (size_t j = 0; j < boundRect.size(); ++j)
    if (boundRect[j].height > boundRect[j].width * 10)
      del[j] = true;
  filter(boundRect, del);


  for (size_t i = 0; i < boundRect.size(); ++i)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); ++i)
    for (size_t j = i + 1; j < boundRect.size(); ++j)
      if ((i != j) && (!del[j]) && (!del[i]) &&
          collide(boundRect[i], boundRect[j]))
      {

        int xmin = std::min(boundRect[i].x, boundRect[j].x);
        int ymin = std::min(boundRect[i].y, boundRect[j].y);
        int xmax = std::max(boundRect[i].x + boundRect[i].width, boundRect[j].x + boundRect[j].width);
        int ymax = std::max(boundRect[i].y + boundRect[i].height, boundRect[j].y + boundRect[j].height);

        boundRect[i].x = xmin;
        boundRect[i].width = xmax - xmin;
        boundRect[i].y = ymin;
        boundRect[i].height = ymax - ymin;

        del[j] = true;
      }
  filter(boundRect, del);

  for (size_t i = 0; i < boundRect.size(); ++i)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); ++i)
    for (size_t j = 0; j < boundRect.size(); ++j)
      if ((i != j) &&
          (fabs(boundRect[i].x + boundRect[i].width - boundRect[j].x) < epsilon) &&
          (boundRect[i].y == boundRect[j].y))
      {
        boundRect[i].width = boundRect[j].x + boundRect[j].width - boundRect[i].x;
        del[j] = true;
      }
  filter(boundRect, del);
}

void dispatch_keys(std::vector<cv::Rect>& pistes,
                   std::vector<Symbol>& symbols,
                   std::vector<Symbol>& keys)
{
  for (size_t k = 0; k < pistes.size(); ++k)
  {
    int min = 100000;
    int imin = 0;

    for (size_t i = 0; i < symbols.size(); ++i)
    {
      if (collide(symbols[i].rect, pistes[k]) &&
          (min > symbols[i].rect.x))
      {
        min = symbols[i].rect.x;
        imin = i;
      }
    }
    keys.push_back(symbols[imin]);
    symbols.erase(symbols.begin() + imin);
  }
}

void remove_pistes(cv::Mat& img,
                   std::vector<cv::Rect>& pistes_rect)
{
  int decidor = 0;
  int min = 0;
  int h = img.size().height;

  for (int k = 0; k < img.size().height; ++k)
  {
    decidor = 0;
    min = h;

    for (size_t piste = 0; piste < pistes_rect.size(); ++piste)
    {
      if ((k >= pistes_rect[piste].y) &&
          (k <= pistes_rect[piste].y + pistes_rect[piste].height))
      {
        min = 0;
        decidor = piste;
        break;
      }
      else if ((k > pistes_rect[piste].y + pistes_rect[piste].height) &&
               (min > fabs(k - pistes_rect[piste].y + pistes_rect[piste].height)))
      {
        min = fabs(k - pistes_rect[piste].y + pistes_rect[piste].height);
        decidor = piste;
      }
      else if ((k < pistes_rect[piste].y) && (min > fabs(k - pistes_rect[piste].y)))
      {
        min = fabs(k - pistes_rect[piste].y);
        decidor = piste;
      }
    }

    for (int x = 0; x < pistes_rect[decidor].x + 2; ++x)
      img.at<uchar>(k, x) = 0;
  }

}

std::vector<cv::Rect> get_symbols_rect(cv::Mat& ret,
                                       std::vector<Line>& lines,
                                       std::vector<cv::Rect>& pistes_rect)
{
  std::vector<cv::Rect> symbols_rect;

  remove_pistes(ret, pistes_rect);
  remove_lines(ret, lines);
  symbols_rect = get_bounding_box(ret);
  filter_bbox(symbols_rect, lines, pistes_rect);
  return symbols_rect;
}

bool by_x(Symbol a,
          Symbol b)
{
  return (a.rect.x <= b.rect.x);
}

void filter_symbols(std::vector<Symbol>& symbols,
                    std::vector<cv::Rect>& pistes_rect)
{
  std::vector<Symbol> sym_in_piste;
  double piste_height = pistes_rect[0].height;
  bool del[symbols.size()];

  for (size_t k = 0; k < pistes_rect.size(); ++k)
  {
    sym_in_piste.clear();
    for (size_t u = 0; u < symbols.size(); ++u)
    {
      del[u] = false;
      if ((int)k == symbols[u].piste)
        sym_in_piste.push_back(symbols[u]);
    }

    mysort(sym_in_piste, by_x);

    for (size_t u = 0; u < sym_in_piste.size(); ++u)
      if ((sym_in_piste[u].rect.height > piste_height * 0.5) &&
          (sym_in_piste[u].rect.width > sym_in_piste[u].rect.height /
           3.0))
      {
        for (size_t i = 0; i < u; ++i)
          for (size_t isym = 0; isym < symbols.size(); ++isym)
            if ((sym_in_piste[i].rect.x == symbols[isym].rect.x) &&
                (sym_in_piste[i].rect.y == symbols[isym].rect.y))
            {
              del[isym] = true;
              break;
            }
        break;
      }
    filter(symbols, del);
  }

}

void detect_symbols(cv::Mat& img,
                    std::vector<Line>& lines,
                    std::vector<Symbol>& symbols,
                    std::vector<cv::Rect>& pistes_rect)
{
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<cv::Rect> symbols_rect;

  symboldetection_preprocess(img, ret, lines);
  pistes_rect = get_piste_rect(lines, ret);
  symbols_rect = get_symbols_rect(ret, lines, pistes_rect);

  for (size_t k = 0; k < symbols_rect.size(); ++k)
  {
    Symbol s;

    s.rect = symbols_rect[k];
    s.pos = -1;
    s.piste = -1;

    for (size_t u = 0; u < pistes_rect.size(); ++u)
      if (collide(s.rect, pistes_rect[u]))
      {
        s.piste = u;
        break;
      }
    symbols.push_back(s);
  }
  filter_symbols(symbols, pistes_rect);
}
