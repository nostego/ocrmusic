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

  for (int x = 0; x < img.size().width; ++x)
  {
    unsigned int vertpix = 0;
    for (int y = 0; y < img.size().height; ++y)
    {
      if (img.at<uchar>(y, x) != 0)
        ++vertpix;
    }
    if (vertpix >= 0.4 * piste_height)
    {
      hasverticalline = true;
      for (int y = 0; y < img.size().height; ++y)
        img.at<uchar>(y, x) = 0;
      IplImage src(img);
      IplConvKernel *kernel;
      kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
      cvErode(&src,&src,kernel, 1);
      cvDilate(&src,&src,kernel, 1);
      //display(img, 70);
    }
  }

  // IsWhite and shapped like a square Or Height >> width And Verticale line.
  // FIXME: Hasverticalline doesn't work when the note is upside down.
  return (/*(isWhite && abs(diff) < 3) || (diff >= 2.75 ||*/( hasverticalline));
}

void analyse_note(cv::Mat& img,
                  std::vector<Line>& lines,
                  cv::Mat& note, int x, int y)
{
  CvFont font;
  double hScale = 1.0;
  double vScale = 1.0;
  int lineWidth = 1;
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);

  //std::string note;
  //display(note, 60);
  unsigned int mid = y + note.size().height / 2;
  // FIXME: not robust if not enough lines.
  unsigned dis;
  int choosen_pitch = -1;
  std::string notestr = "";
  for (unsigned int i = 0; i < lines.size() - 1; ++i)
  {
    /*for (unsigned int x2 = 0; x2 < img.size().width * 3; ++x2)
      img.at<uchar>(lines[i].y, x) = 0;*/

    // FIXME: improvement when note is outside of the line.
    if (mid <= lines[i + 1].y && mid >= lines[i].y)
    {
      dis = lines[i + 1].y - lines[i].y;
      std::cout << dis << " i: " << abs(lines[i].y - mid) << std::endl;
      std::cout << dis << " i + 1: " << abs(lines[i + 1].y - mid) << std::endl;
      if (abs(lines[i].y - mid) < dis / 2)
      {
        std::cout << "CHOOSE i !\n";
        choosen_pitch = i % 10;
      }
      else if (abs(lines[i + 1].y - mid) < dis / 2)
      {
        std::cout << "CHOOSE i + 1 !\n";
        choosen_pitch = (i + 1) % 10;
      }
    }

    if (choosen_pitch != -1)
    {
      if (choosen_pitch == 0)
        notestr = "F";
      else if (choosen_pitch == 1)
        notestr = "D";
      else if (choosen_pitch == 2)
        notestr = "B";
      else if (choosen_pitch == 3)
        notestr = "G";
      else if (choosen_pitch == 4)
        notestr = "E";

      else if (choosen_pitch == 5)
        notestr = "A";
      else if (choosen_pitch == 6)
        notestr = "F";
      else if (choosen_pitch == 7)
        notestr = "D";
      else if (choosen_pitch == 8)
        notestr = "B";
      else if (choosen_pitch == 9)
        notestr = "G";
    }
    else
    {
      if (i == 0)
        notestr = "E";
      else if (i == 1)
        notestr = "C";
      else if (i == 2)
        notestr = "A";
      else if (i == 3)
        notestr = "F";

      else if (i == 5)
        notestr = "G";
      else if (i == 6)
        notestr = "E";
      else if (i == 7)
        notestr = "C";
      else if (i == 8)
        notestr = "A";
    }
  }
  cv::putText(img, notestr, cvPoint(x, y - 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, 255);
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
