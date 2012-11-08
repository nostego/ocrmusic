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

  remove_pistes(ret, pistes);
  remove_lines(ret, lines);
}

bool isnote(cv::Mat& img,
	    int piste_height)
{
  bool hasverticalline = false;

  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(img, lines, 1, CV_PI/180, 50, 50, 10);
  for(size_t i = 0; i < lines.size(); i++)
  {
    cv::Vec4i l = lines[i];
    line(img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
    display(img, 70);
    hasverticalline = true;
  }

  float diff = (float) img.size().height / (float) img.size().width;
  bool isWhite = img.at<uchar>(img.size().height / 2, img.size().width / 2) == 0;

  /*if (hasverticalline)
  {
    IplImage src(img);
    IplConvKernel *kernel;
    kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
    cvErode(&src,&src,kernel, 2);
    cvDilate(&src,&src,kernel, 2);
  }*/

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
    {
      std::vector<cv::Rect> note = get_bounding_box(symbol_img);
      for (unsigned int i = 0; i < note.size(); ++i)
      {
        img.at<uchar>(symbols[k].rect.y + note[i].y + note[i].height / 2, symbols[k].rect.x + note[i].x + note[i].width / 2) = 0xff0000;
        note[i].x = symbols[k].rect.x + note[i].x;
        note[i].y = symbols[k].rect.y + note[i].y;
      }
      display_rect(img, note, 0x00ffff);
      // For debug.
      // display(symbol_img, 70);
     // display_onerect(img, symbols[k].rect, 0x0000ff);
    }
    else
      display_onerect(img, symbols[k].rect, 0xff0000);
  }
}
