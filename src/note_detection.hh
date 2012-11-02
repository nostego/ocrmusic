#ifndef NOTE_DETECTION_HH_
# define NOTE_DETECTION_HH_

# include "symbol_detection.hh"

struct Note : public Symbol
{
  int note;
};

void detect_notes(cv::Mat& img,
		  std::vector<Line>& lines,
		  std::vector<cv::Rect>& pistes,
		  std::vector<Symbol>& symbols,
		  std::vector<Note>& notes);
#endif // !NOTE_DETECTION_HH_
