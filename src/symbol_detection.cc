#include "symbol_detection.hh"
#include "tools.hh"
#include "ocr.hh"

void symboldetection_preprocess(cv::Mat& img,
                                cv::Mat& ret,
                                std::vector<Line>& lines)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 195.0, 255.0, cv::THRESH_BINARY_INV);
  remove_lines(ret, lines);

  cv::dilate(ret, ret, cv::Mat(cv::Size(2, 2), CV_8UC1));
  cv::erode(ret, ret, cv::Mat(cv::Size(2, 2), CV_8UC1));

  //display(ret, 600);
}

std::vector<cv::Rect> get_piste_rect(std::vector<Line>& lines,
                                     cv::Mat& img)
{
  std::vector<cv::Rect> rect;
  double piste_height = fabs(lines[4].y - lines[0].y);
  double interlap = piste_height / 4.0;

  for (size_t k = 0; k < lines.size(); k += 5)
  {
    cv::Rect r;

    r.x = 0;
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
  double piste_height = fabs(lines[4].y - lines[0].y);

  for (size_t i = 0; i < boundRect.size(); ++i)
  {
    bool onpiste = false;

    del[i] = (boundRect[i].height > piste_height * 2.0) ||
      (boundRect[i].x < 10);
    
    for (size_t u = 0; u < pistes_rect.size(); ++u)
      onpiste |= collide(pistes_rect[u], boundRect[i]);
    del[i] |= !onpiste;
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

std::vector<cv::Rect> get_symbols_rect(cv::Mat& ret,
                                       std::vector<Line>& lines,
                                       std::vector<cv::Rect>& pistes_rect)
{
  std::vector<cv::Rect> symbols_rect;

  symbols_rect = get_bounding_box(ret);
  filter_bbox(symbols_rect, lines, pistes_rect);
  return symbols_rect;
}

std::vector<Symbol> detect_symbols(cv::Mat& img,
                                   std::vector<Line>& lines)
{
  Ocr* ocr = 0;
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<Symbol> symbols;
  std::vector<cv::Rect> symbols_rect;
  std::vector<cv::Rect> pistes_rect;

  symboldetection_preprocess(img, ret, lines);
  pistes_rect = get_piste_rect(lines, ret);
  symbols_rect = get_symbols_rect(ret, lines, pistes_rect);

  display_rect(img, symbols_rect, 0x0000ff);
  display_rect(img, pistes_rect, 0xff0000);
  for (size_t k = 0; k < symbols_rect.size(); ++k)
  {
    Symbol s;

    s.rect = symbols_rect[k];
    s.pos = -1;
    symbols.push_back(s);
  }
  return symbols;
}
