#include "note_detection.hh"
#include "tools.hh"

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
  for (size_t i = 0; i < contours.size(); i++ )
  {
    cv::Point pt1;
    cv::Point pt2;

    approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
    boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
    pt1.x = boundRect[i].x;
    pt1.y = boundRect[i].y;
    pt2.x = boundRect[i].x + boundRect[i].width;
    pt2.y = boundRect[i].y + boundRect[i].height;

    cv::rectangle(img, pt1, pt2, sc);
  }
  display(img, 800);
  return notes;
}
