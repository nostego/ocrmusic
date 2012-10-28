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

std::vector<cv::Rect> get_piste_rect(std::vector<Line>& lines,
				     cv::Mat& img)
{
  std::vector<cv::Rect> rect;
  double piste_height = fabs(lines[4].y - lines[0].y);
  double interlap = piste_height / 4.0;

  for (size_t k = 0; k < lines.size(); k += 5)
  {
    cv::Rect r;

    r.x = 0;
    r.width = img.size().width;
    r.y = lines[k].y - interlap;
    r.height = piste_height + interlap * 2;
    rect.push_back(r);
  }

  return rect;
}

void filter_bbox(std::vector<cv::Rect>& boundRect,
		 std::vector<Line>& lines,
		 std::vector<cv::Rect>& pistes_rect)
{
  bool del[boundRect.size()];
  double piste_height = fabs(lines[4].y - lines[0].y);

  for (size_t i = 0; i < boundRect.size(); ++i)
  {
    del[i] = (boundRect[i].height > piste_height * 2.0);

    bool onpiste = false;

    for (size_t u = 0; u < pistes_rect.size(); ++u)
    {
      onpiste |= collide(pistes_rect[u], boundRect[i]);
    }
    del[i] |= !onpiste;
  }
  filter(boundRect, del);
}

std::vector<cv::Rect> get_notes_rect(cv::Mat& ret,
				     std::vector<Line>& lines,
				     std::vector<cv::Rect>& pistes_rect)
{
  std::vector<cv::Rect> notes_rect;

  notes_rect = get_bounding_box(ret);  
  filter_bbox(notes_rect, lines, pistes_rect);
  return notes_rect;
}

std::vector<Note> detect_notes(cv::Mat& img,
                               std::vector<Line>& lines)
{
  cv::Mat ret(img.size(), CV_8UC1);
  std::vector<Note> notes;
  std::vector<cv::Rect> notes_rect;
  std::vector<cv::Rect> pistes_rect;

  notedetection_preprocess(img, ret, lines);
  pistes_rect = get_piste_rect(lines, ret);
  notes_rect = get_notes_rect(ret, lines, pistes_rect);

  display_rect(img, notes_rect, 0x0000ff);
  display_rect(img, pistes_rect, 0xff0000);
  cv::imwrite("output.png", img);
  display(img, 800);

  return notes;
}
