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

    for (int u = 0; u < img.size().width; ++u)
      tmp[u] = 0;

    for (int x = 0; x < img.size().width; ++x)
    {
      for (int range = -drange; range < drange; ++range)
        if (isvalid(img, x + range, lines[k].y))
          tmp[x] += img.at<uchar>(lines[k].y, x + range) / 255;
    }

    r.x = 0;
    while (r.x + drange < img.size().width)
    {
      if (tmp[r.x + drange] >= drange * 2)
        break;
      ++r.x;
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
          (boundRect[j].y == boundRect[i].y))
        del[j] = true;
  filter(boundRect, del);
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
  cv::dilate(ret, ret, cv::Mat(cv::Size(5, 5), CV_8UC1));
  symbols_rect = get_bounding_box(ret);
  filter_bbox(symbols_rect, lines, pistes_rect);
  return symbols_rect;
}

std::vector<Symbol> detect_symbols(cv::Mat& img,
                                   std::vector<Line>& lines)
{
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<Symbol> symbols;
  std::vector<cv::Rect> symbols_rect;
  std::vector<cv::Rect> pistes_rect;

  symboldetection_preprocess(img, ret, lines);
  pistes_rect = get_piste_rect(lines, ret);
  symbols_rect = get_symbols_rect(ret, lines, pistes_rect);

  for (size_t k = 0; k < symbols_rect.size(); ++k)
  {
    Symbol s;

    s.rect = symbols_rect[k];
    s.pos = -1;
    symbols.push_back(s);
  }

  //display_lines(img, lines, 0x00ff00);
  display_rect(img, symbols_rect, 0x0000ff);
  display_rect(img, pistes_rect, 0xff0000);
  //display(img, 700);

  return symbols;
}
