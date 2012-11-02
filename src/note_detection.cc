#include "note_detection.hh"
#include "symbol_detection.hh"
#include "tools.hh"

void notedetection_preprocess(cv::Mat& img,
                              cv::Mat& ret,
                              std::vector<Line>& lines,
                              std::vector<cv::Rect>& pistes)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 195.0, 255.0, cv::THRESH_BINARY_INV);

  remove_lines(ret, lines);
  remove_pistes(ret, pistes);
}

bool isnote(cv::Mat& img,
	    int piste_height)
{
  bool hasverticalline = false;

  cv::dilate(img, img, cv::Mat(cv::Size(2, 2), CV_8UC1));
  for (int x = 0; x < img.size().width; ++x)
  {
    int longest = 0;
    int current = 0;

    for (int y = 0; y < img.size().height; ++y)
    {
      if (255 == img.at<uchar>(y, x))
	++current;
      else
      {
	longest = std::max(longest, current);
	current = 0;
      }
    }
    if (longest >= 0.5 * piste_height)
    {
      hasverticalline = true;
      break;
    }
  }
  return hasverticalline;
}

void detect_notes(cv::Mat& img,
                  std::vector<Line>& lines,
                  std::vector<cv::Rect>& pistes,
                  std::vector<Symbol>& symbols,
                  std::vector<Note>& notes)
{
  cv::Mat ret(img.size(), CV_8UC1);

  notedetection_preprocess(img, ret, lines, pistes);

  for (size_t k = 0; k < symbols.size(); ++k)
  {
    cv::Mat symbol_img(cv::Size(symbols[k].rect.width,
				symbols[k].rect.height),
		       CV_8UC1);
    for (int y = 0; y < symbol_img.size().height; ++y)
      for (int x = 0; x < symbol_img.size().width; ++x)
	symbol_img.at<uchar>(y, x) = ret.at<uchar>(y + symbols[k].rect.y, x + symbols[k].rect.x);
    if (isnote(symbol_img, pistes[0].height))
      display_onerect(img, symbols[k].rect, 0x0000ff);
    else
      display_onerect(img, symbols[k].rect, 0xff0000);
  }
  //display(img, 700);
  (void)notes;
}
