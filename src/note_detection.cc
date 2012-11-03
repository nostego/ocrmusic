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
  imshow("lol", ret);
}
/*
int count_ellipse(cv::Mat& img,
		   int piste_height)
{
  int er = piste_height / 12 - 1;
  er = 5;
  cv::Mat ret(img.clone());

  std::cout << "er = " << er << std::endl;
  myerode(img, ret, cv::Size(er, er));
  cv::dilate(ret, ret, cv::Mat(cv::Size(er, er), CV_8UC1));
  //mydilate(ret, ret, cv::Size(2, 2));
  std::vector<cv::Rect> ellipses = get_bounding_box(ret);


  bool del[ellipses.size()];

  for (size_t k = 0; k < ellipses.size(); ++k)
  {
    del[k] = ellipses[k].width > ellipses[k].height * 2;
  }
  //filter(ellipses, del);
  std::cout << ellipses.size() << std::endl;
  //display(ret);
  return ellipses.size();
}
*/
bool isnote(cv::Mat& img,
	    int piste_height)
{
  bool hasverticalline = false;
  int current;
  int longest;

  for (int x = 0; x < img.size().width; ++x)
  {
    current = 0;
    longest = 0;

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
  int diff = img.size().width - img.size().height;
  bool isWhite = img.at<uchar>(img.size().height / 2, img.size().width / 2) == 0;
  return (isWhite && (diff < 25 && diff > 0)) || hasverticalline;
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
}
