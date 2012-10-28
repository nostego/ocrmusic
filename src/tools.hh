#ifndef TOOLS_HH_
# define TOOLS_HH_

# include <opencv/cv.h>
# include <opencv/highgui.h>
# include <vector>

void display(cv::Mat& img,
             double target_display_w);

template <typename T>
void filter(std::vector<T>& v,
            bool* mask)
{
  int offset = 0;
  size_t si = v.size();

  for (size_t k = 0; k < si; ++k)
  {
    if (mask[k])
    {
      v.erase(v.begin() + k - offset);
      ++offset;
    }
  }
}

std::vector<cv::Rect> get_bounding_box(cv::Mat& img);
void display_rect(cv::Mat& img,
		  std::vector<cv::Rect>& boundRect,
		  int rgb);
bool collide(cv::Rect& a, cv::Rect& b);

#endif // !TOOLS_HH_
