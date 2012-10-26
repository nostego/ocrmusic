#include "line_detection.hh"
#include "tools.hh"

#define MARGIN_SCAN 0.01
#define EPSILON 0.01

using namespace cv;

std::vector<Line> detect_lines(cv::Mat& img,
                               double max_rot)
{
  std::vector<Line> mylines;
  cv::Mat ret(img.size(), CV_8UC1);

  linedetection_preprocess(img, ret);
  mylines = get_raw_lines(ret, max_rot);
  return mylines;
}

void linedetection_preprocess(cv::Mat& img,
                              cv::Mat& ret)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 200.0, 255.0, cv::THRESH_BINARY_INV);
  //cv::dilate(ret, ret, cv::Mat(cv::Size(2, 2), CV_8UC1));
  //cv::erode(ret, ret, cv::Mat(cv::Size(2, 2), CV_8UC1));
  //display(ret, 600);
}

std::vector<Line> get_raw_lines(cv::Mat& img, double max_rot)
{
  std::vector<Line> mylines;
  std::vector<Vec2f> lines;

  HoughLines(img, lines, 1, CV_PI / 180.0, 20);
  for (size_t i = 0; i < lines.size(); ++i)
  {
    if ((fabs(lines[i][1] - (M_PI / 2.0)) < max_rot * (M_PI / 180.0)) &&
        (lines[i][0] >= MARGIN_SCAN * img.size().height) &&
        (lines[i][0] <= img.size().height - MARGIN_SCAN * img.size().height))
    {
      int energy = 0.0;
      double a = (1.0 / ((-2.0 / M_PI) * lines[i][1])) + 1;

      for (int x = 0; x < img.size().width; ++x)
      {
        int y = a * x + lines[i][0];

        if ((y >= 0) && (y < img.size().height))
          energy += (int)img.at<uchar>(y, x) / 255;
      }
      if (energy >= 0.6 * img.size().width)
      {
        Line n;

        n.y = lines[i][0];
        n.h = 1;
        n.angle = lines[i][1];
        mylines.push_back(n);
      }
    }
  }


  return mylines;
}

void display_lines(cv::Mat& img,
                   std::vector<Line>& mylines)
{
  for (size_t i = 0; i < mylines.size(); ++i)
  {
    Vec4i l;
    double a = (1.0 / ((-2.0 / M_PI) * mylines[i].angle)) + 1;

    l[0] = 0;
    l[1] = mylines[i].y;
    l[2] = img.size().width;
    l[3] = a * img.size().width + mylines[i].y;
    line(img, Point(l[0], l[1]),
         Point(l[2], l[3]), Scalar(0,0,255), 1, 8 );
  }
}

