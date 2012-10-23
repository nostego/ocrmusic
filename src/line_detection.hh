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

bool sort_line(cv::Vec4i i, cv::Vec4i j);
bool unique_line(cv::Vec4i i, cv::Vec4i j);
std::vector<cv::Vec4i> detect_lines(cv::Mat& img);
void remove_lines(cv::Mat& img,
                  std::vector<cv::Vec4i>& lines);
void display_lines(cv::Mat& img,
                   std::vector<cv::Vec4i>& lines);
#endif // !LINE_DETECTION_HH_
