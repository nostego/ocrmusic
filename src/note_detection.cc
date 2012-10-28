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

void filter_bbox(std::vector<cv::Rect>& boundRect,
		 cv::Mat& img,
		 std::vector<Line>& lines)
{
  bool del[boundRect.size()];
  bool inpiste[img.size().height];
  double piste_height = fabs(lines[4].y - lines[0].y);
  double interlap = piste_height / 4.0;
 
  for (int i = 0; i < img.size().height; i++)
    inpiste[i] = false;

  for (size_t i = 0; i < lines.size(); i++)
    for (int y = lines[i].y - interlap;
         y < lines[i].y + interlap;
         ++y)
      if ((y >= 0) && (y < img.size().height))
        inpiste[y] = true;

  for (size_t i = 0; i < boundRect.size(); i++)
  {
    del[i] = (boundRect[i].height > piste_height * 2.0) ||
      (!inpiste[boundRect[i].y] &&
       !inpiste[boundRect[i].y + boundRect[i].height] &&
       !inpiste[boundRect[i].y + boundRect[i].height / 2]);
  }
  filter(boundRect, del);
}

std::vector<Note> detect_notes(cv::Mat& img,
                               std::vector<Line>& lines)
{
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<Note> notes;
  std::vector<cv::Rect> boundRect;

  notedetection_preprocess(img, ret, lines);
  boundRect = get_bounding_box(ret);
  filter_bbox(boundRect, img, lines);

  display_rect(img, boundRect);
  cv::imwrite("output.png", img);
  display(img, 800);

  return notes;
}
