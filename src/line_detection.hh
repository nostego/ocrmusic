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

std::vector<Line> get_raw_lines(cv::Mat& img, double max_rot);
std::vector<Line> detect_lines(cv::Mat& img, double max_rot);

void display_lines(cv::Mat& img,
                   std::vector<Line>& lines);
#endif // !LINE_DETECTION_HH_
