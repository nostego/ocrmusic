#ifndef LINE_DETECTION_HH_
# define LINE_DETECTION_HH_

# include <opencv/cv.h>
# include <opencv/highgui.h>
# include <vector>
# include <algorithm>
# include <cmath>
# include <iostream>

struct Line
{
  size_t y;
  size_t h;
  double angle;
};

double get_raw_lines(std::vector<Line>& mylines,
		     cv::Mat& img, double max_rot,
		     bool is_rot);
std::vector<Line> detect_lines(cv::Mat& img, double max_rot);

void linedetection_preprocess(cv::Mat& img,
			      cv::Mat& ret);
void filter_lines(std::vector<Line>& lines,
		  int height);
void display_lines(cv::Mat& img,
                   std::vector<Line>& lines,
		   int rgb);

void remove_lines(cv::Mat& img,
		  std::vector<Line>& lines);
#endif // !LINE_DETECTION_HH_
