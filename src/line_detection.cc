#include "line_detection.hh"
#define EPSILON 0.00001
#define MARGIN_SCAN 0.01
#define MAX_ROT 1

using namespace cv;

std::vector<Line> get_raw_lines(cv::Mat& img)
{
  std::vector<Line> mylines;
  std::vector<Vec2f> lines;

  HoughLines(img, lines, 1, CV_PI / 180.0, 20);
  for (size_t i = 0; i < lines.size(); ++i)
  {
    if ((fabs(lines[i][1] - (M_PI / 2.0)) < EPSILON) &&
	(lines[i][0] >= MARGIN_SCAN * img.size().height) &&
	(lines[i][0] <= img.size().height - MARGIN_SCAN * img.size().height)) // TO modify for rot
    {
      int energy = 0.0;

      for (int x = 0; x < img.size().width; ++x)
      {
        energy += (int)img.at<uchar>(lines[i][0], x) / 255;
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

std::vector<Line> detect_lines(cv::Mat& img)
{

  std::vector<Vec4i> ret;
  std::vector<Line> mylines;

  mylines = get_raw_lines(img);

  return mylines;
}

void display_lines(cv::Mat& img,
                   std::vector<Line>& mylines)
{
  for (size_t i = 0; i < mylines.size(); ++i)
  {
    for (size_t hi = 0; hi < mylines[i].h; ++hi)
    {
      Vec4i l;

      l[0] = 0;
      l[1] = mylines[i].y + hi;
      l[2] = img.size().width;
      l[3] = mylines[i].y + hi;
      line(img, Point(l[0], l[1]),
           Point(l[2], l[3]), Scalar(0,0,255), 1, 8 );
    }
  }
}

