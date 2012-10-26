#include "note_detection.hh"
#include "tools.hh"

#define EPSILON 0.01
void notedetection_preprocess(cv::Mat& img,
                              cv::Mat& ret,
                              std::vector<Line>& lines)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 195.0, 255.0, cv::THRESH_BINARY_INV);
  remove_lines(ret, lines);
}


std::vector<Note> detect_notes(cv::Mat& img,
                               std::vector<Line>& lines)
{
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<Note> notes;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  notedetection_preprocess(img, ret, lines);
  findContours(ret, contours, hierarchy,
               CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

  std::vector<cv::Rect> boundRect(contours.size() );
  std::vector<std::vector<cv::Point> > contours_poly(contours.size());

  cv::Scalar sc;

  sc[0] = 255;
  sc[1] = 0;
  sc[2] = 0;

  double piste_height = fabs(lines[4].y - lines[0].y);
  double interlap = piste_height / 4.0;

  for (size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
    boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
  }

  bool del[boundRect.size()];

  for (size_t i = 0; i < boundRect.size(); i++)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); i++)
    for (size_t j = 0; j < boundRect.size(); j++)
    {

      if ((i != j) && (boundRect[i].x >= boundRect[j].x) &&
          (boundRect[i].x + boundRect[i].width
           <= boundRect[j].x + boundRect[j].width) &&
          (boundRect[i].y >= boundRect[j].y) &&
          (boundRect[i].y + boundRect[i].height
           <= boundRect[j].y + boundRect[j].height))
        del[i] = true;
    }
  size_t si = boundRect.size();
  int offset = 0;

  for (size_t i = 0; i < si; i++)
  {
    if (del[i])
    {
      boundRect.erase(boundRect.begin() + i - offset);
      ++offset;
    }
  }

  for (size_t i = 0; i < boundRect.size(); i++)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); i++)
  {
    if ((double)boundRect[i].height > piste_height * 2.0)
      del[i] = true;
  }

  si = boundRect.size();
  offset = 0;

  for (size_t i = 0; i < si; i++)
  {
    if (del[i])
    {
      boundRect.erase(boundRect.begin() + i - offset);
      ++offset;
    }
  }

  bool inpiste[img.size().height];

  for (size_t i = 0; i < img.size().height; i++)
    inpiste[i] = false;

  for (size_t i = 0; i < lines.size(); i++)
  {
    for (int y = lines[i].y - interlap;
	 y < lines[i].y + interlap;
	 ++y)
    {
      if ((y >= 0) && (y < img.size().height))
	inpiste[y] = true;
    }
  }

  for (size_t i = 0; i < boundRect.size(); i++)
    del[i] = false;
  for (size_t i = 0; i < boundRect.size(); i++)
  {
    if (!inpiste[boundRect[i].y] &&
	!inpiste[boundRect[i].y + boundRect[i].height] &&
	!inpiste[boundRect[i].y + boundRect[i].height / 2])
      del[i] = true;
  }

  si = boundRect.size();
  offset = 0;

  for (size_t i = 0; i < si; i++)
  {
    if (del[i])
    {
      boundRect.erase(boundRect.begin() + i - offset);
      ++offset;
    }
  }

  for (size_t i = 0; i < boundRect.size(); i++)
  {
    cv::Point pt1;
    cv::Point pt2;

    pt1.x = boundRect[i].x;
    pt1.y = boundRect[i].y;
    pt2.x = boundRect[i].x + boundRect[i].width;
    pt2.y = boundRect[i].y + boundRect[i].height;

    cv::rectangle(img, pt1, pt2, sc);
  }
  cv::imwrite("output.png", img);
  display(img, 800);
  return notes;
}
