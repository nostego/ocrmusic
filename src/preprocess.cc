#include "preprocess.hh"

cv::Mat preprocess(cv::Mat& img)
{
  cv::Mat ret(img.size(), CV_8UC1);

  cv::cvtColor(img, ret, CV_RGB2GRAY);
  //cv::equalizeHist(ret, ret);
  cv::threshold(ret, ret, 190.0, 255.0, cv::THRESH_BINARY_INV);
  return ret;
}

