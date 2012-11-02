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
  filter_lines(mylines, img.size().height);

  return mylines;
}
bool order_y(Line a,
             Line b)
{
  return a.y < b.y;
}

void filter_lines(std::vector<Line>& lines,
                  int height)
{
  double dist[4];
  bool todel[lines.size()];
  double mean;

  sort(lines.begin(), lines.end(), order_y);

  for (size_t i = 0; i < lines.size(); ++i)
    todel[i] = true;


  int lasty = -1;
  int lastk = 0;

  for (size_t k = 0; k < lines.size(); ++k)
  {
    if ((lasty < 0) ||
        (fabs(lasty - (double)lines[k].y) >  1 + height * 0.001))
    {
      todel[k] = false;
      lastk = k;
    }
    else
      ++lines[lastk].h;
    lasty = lines[k].y;
  }

  filter(lines, todel);
  for (size_t i = 0; i < lines.size(); ++i)
    todel[i] = true;

  for (size_t i = 0; i < lines.size() - 4; ++i)
  {
    bool is_valid = true;

    mean = 0.0;
    for (size_t k = 0; k < 4; ++k)
    {
      dist[k] = lines[1 + i + k].y - lines[i + k].y;
      mean += dist[k];
    }
    mean /= 4.0;
    for (size_t k = 0; k < 4; ++k)
    {
      dist[k] -= mean;
      if (fabs(dist[k]) > 1.0 + (height / 100.0))
        is_valid = false;
    }
    if (is_valid)
      for (size_t k = 0; k < 5; ++k)
        todel[i + k] = false;
  }

  filter(lines, todel);
}

void linedetection_preprocess(cv::Mat& img,
                              cv::Mat& ret)
{
  cv::cvtColor(img, ret, CV_RGB2GRAY);
  cv::threshold(ret, ret, 195.0, 255.0, cv::THRESH_BINARY_INV);
}

void remove_lines(cv::Mat& img,
                  std::vector<Line>& lines)
{
  Scalar sc;

  sc[0] = 255;
  cv::Mat mask(img.size(), CV_8UC1, sc);

  display_lines(mask, lines, 0xff0000);
  cv::threshold(mask, mask, 150.0, 255.0, cv::THRESH_BINARY_INV);
  cv::inpaint(img, mask, img, 2, INPAINT_TELEA | INPAINT_NS);
   //lesser thresh = more line
  cv::threshold(img, img, 80.0, 255.0, cv::THRESH_BINARY);
  //display(img, 700);
}

std::vector<Line> get_raw_lines(cv::Mat& img, double max_rot)
{
  std::vector<Line> mylines;
  std::vector<Vec2f> lines;

  HoughLines(img, lines, 1, CV_PI / 180.0, 10);
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
                   std::vector<Line>& mylines,
                   int rgb)
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
         Point(l[2], l[3]),
         Scalar(rgb & 0x0000ff, rgb & 0x00ff00, rgb & 0xff0000), 1 + img.size().height / 1000,
         8);
  }
}

