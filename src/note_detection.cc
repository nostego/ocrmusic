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
  int current;
  int longest;
  int lonlongest = 0;

  for (int x = 0; x < img.size().width; ++x)
  {
    current = 0;
    longest = 0;

    for (int y = 0; y < img.size().height; ++y)
    {
      if (255 == img.at<uchar>(y, x))
      {
	++current;
      }
      else
      {
        img.at<uchar>(y, x) = 0;
	longest = std::max(longest, current);
        lonlongest = std::max(longest, lonlongest);
        current = 0;
      }
    }
    for (int y = img.size().height - 1; y >= 0; --y)
    {
      if (255 == img.at<uchar>(y, x))
      {
	++current;
      }
      else
      {
        img.at<uchar>(y, x) = 0;
	longest = std::max(longest, current);
        lonlongest = std::max(longest, lonlongest);
        current = 0;
      }
    }
    if (longest >= 0.5 * piste_height)
    {
      hasverticalline = true;
      break;
    }
  }

  float diff = (float) img.size().height / (float) img.size().width;
  // FIXME: search in the MIDDLE of the note, not in the image.
  // FIXME: search if you have a Circle note !
  bool isWhite = img.at<uchar>(img.size().height / 2, img.size().width / 2) == 0;

  // IsWhite and shapped like a square Or Height >> width And Verticale line.
  return ((isWhite && abs(diff) < 3) || (diff >= 2.75 || hasverticalline));
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
  (void)notes;
}
