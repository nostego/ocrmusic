#include "tools.hh"

void display(cv::Mat& img,
             double target_display_w)
{
  cv::Mat disp(cv::Size(target_display_w,
                        (target_display_w / (double)img.size().width)* img.size().height),
               CV_8UC1);
  cv::resize(img, disp, disp.size(), 0, 0, cv::INTER_LINEAR);
  cv::namedWindow("Display");
  cv::imshow("Display", disp);
  cv::waitKey(0);
}
