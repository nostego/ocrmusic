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

  float diff = (float) img.size().height / (float) img.size().width;
  bool isWhite = img.at<uchar>(img.size().height / 2, img.size().width / 2) == 0;

  std::vector<cv::Vec2f> lines;
  cv::Mat res = img;
  cv::HoughLines(res, lines, 1, CV_PI/180, 10);
  for(size_t i = 0; i < lines.size(); i++)
  {
    float rho = lines[i][0];
    float theta = lines[i][1];
    double a = cos(theta), b = sin(theta);
    cv::Point pt1;
    cv::Point pt2;
    bool first = false;
    bool last = false;
    for (int x = 0; x < img.size().width && !last; ++x)
    {
      int y = ((double) -a / (double)b) * x + (rho / b);
      if (y >= 0 && x >= 0 &&
          y < img.size().height && x < img.size().width && img.at<uchar>(y, x))
      {
        if (!first)
        {
          first = true;
          pt1.x = x;
          pt1.y = y;
        }
        pt2.x = x;
        pt2.y = y;
      }
      else
        last = true;
    }
    double dist = sqrt(pow(pt1.x - pt2.x, 2) +
                       pow(pt1.y - pt2.y, 2));
    double angle = abs(theta * 180 / CV_PI);
    // FIXME: don't work for some OBVIOUS lines ??
    if (dist >= 0.4 * piste_height && angle >= 0 && angle <= 10)
    {
      std::cout << dist << std::endl;
      line(res, pt1, pt2, cv::Scalar(0, 255, 255), 3, 8);
      img = res;
      IplImage src(img);
      IplConvKernel *kernel;
      kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
      cvErode(&src,&src,kernel, 2);
      cvDilate(&src,&src,kernel, 3);
      // For debug.
      //display(img, 70);
      hasverticalline = true;
    }

  }

  // IsWhite and shapped like a square Or Height >> width And Verticale line.
  return ((isWhite && abs(diff) < 3) || (diff >= 2.75 || hasverticalline));
}

void analyse_note(cv::Mat& img,
                  std::vector<Line>& lines,
                  cv::Mat& note)
{
  //display(note, 60);
  for (unsigned int i = 0; i < lines.size(); ++i)
  {
    for (unsigned int x = 0; x < img.size().width; ++x)
      note.at<uchar>(x, lines[i].y) = 0;
    //std::cout << lines[i].y << std::endl;
  }
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
      std::vector<cv::Rect> notebb = get_bounding_box(symbol_img);
      for (unsigned int i = 0; i < notebb.size(); ++i)
      {
        notebb[i].x = symbols[k].rect.x + notebb[i].x;
        notebb[i].y = symbols[k].rect.y + notebb[i].y;
        cv::Mat note(cv::Size(notebb[i].width,
                              notebb[i].height), CV_8UC1);
        for (int y = 0; y < note.size().height; ++y)
          for (int x = 0; x < note.size().width; ++x)
            note.at<uchar>(y, x) = symbol_img.at<uchar>(y, x);
        analyse_note(img, lines, note);
        //display_onerect(img, notebb[i], 0x0000ff);
      }
      //display_rect(img, notebb, 0xff00ff);
      // For debug.
      // display(symbol_img, 70);
     // display_onerect(img, symbols[k].rect, 0x0000ff);
    }
    else
      display_onerect(img, symbols[k].rect, 0xff0000);
  }
}
