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
      // FIXME: for debug.
      //std::cout << dist << std::endl;
      //line(res, pt1, pt2, cv::Scalar(0, 255, 255), 3, 8);
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
  // FIXME: Hasverticalline doesn't work when the note is upside down.
  return ((isWhite && abs(diff) < 3) || (diff >= 2.75 || hasverticalline));
}

void analyse_note(cv::Mat& img,
                  std::vector<Line>& lines,
                  cv::Mat& note, int x, int y)
{
  //display(note, 60);
  unsigned int mid = y + note.size().height / 2;
  // FIXME: not robust if not enough lines.
  unsigned dis;
  int choosen_pitch = -1;
  for (unsigned int i = 0; i < lines.size() - 1; ++i)
  {
    for (unsigned int x = 0; x < img.size().width * 3; ++x)
      img.at<uchar>(lines[i].y, x) = 0;

    // FIXME: improvement when note is outside of the line.
    if (mid <= lines[i + 1].y && mid >= lines[i].y)
    {
      dis = lines[i + 1].y - lines[i].y;
      if (abs(lines[i].y - y) < dis)
        choosen_pitch = i % 10;
      else if (abs(lines[i + 1].y - y) < dis)
        choosen_pitch = i % 10;
    }
    // Do the math for the distance between each line to find other notes.
    if (mid <= lines[i + 1].y && mid >= lines[i].y)
    {
      if (choosen_pitch != -1)
      {
        if (choosen_pitch == 0)
          std::cout << "F" << std::endl;
        else if (choosen_pitch == 1)
          std::cout << "D" << std::endl;
        else if (choosen_pitch == 2)
          std::cout << "B" << std::endl;
        else if (choosen_pitch == 3)
          std::cout << "G" << std::endl;
        else if (choosen_pitch == 4)
          std::cout << "F" << std::endl;

        else if (choosen_pitch == 5)
          std::cout << "A" << std::endl;
        else if (choosen_pitch == 6)
          std::cout << "F" << std::endl;
        else if (choosen_pitch == 7)
          std::cout << "D" << std::endl;
        else if (choosen_pitch == 8)
          std::cout << "B" << std::endl;
        else if (choosen_pitch == 9)
          std::cout << "G" << std::endl;
      }
      else
      {
        if (i == 0)
          std::cout << "E" << std::endl;
        else if (i == 1)
          std::cout << "C" << std::endl;
        else if (i == 2)
          std::cout << "A" << std::endl;
        else if (i == 3)
          std::cout << "F" << std::endl;
        /*else if (i == 4)
          std::cout << "C" << std::endl;*/

        else if (i == 5)
          std::cout << "G" << std::endl;
        else if (i == 6)
          std::cout << "E" << std::endl;
        else if (i == 7)
          std::cout << "C" << std::endl;
        else if (i == 8)
          std::cout << "A" << std::endl;
      }
    }
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
        analyse_note(img, lines, note, notebb[i].x, notebb[i].y);
        display_onerect(img, notebb[i], 0x0000ff);
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
