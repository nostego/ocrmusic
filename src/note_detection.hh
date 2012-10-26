#ifndef NOTE_DETECTION_HH_
# define NOTE_DETECTION_HH_

# include "line_detection.hh"

struct Note
{
  int pos;
  int duration;
  int note;
};

void notedetection_preprocess(cv::Mat& img,
			      cv::Mat& ret,
			      std::vector<Line>& lines);

std::vector<Note> detect_notes(cv::Mat& img,
			       std::vector<Line>& lines);

#endif // !NOTE_DETECTION_HH_
