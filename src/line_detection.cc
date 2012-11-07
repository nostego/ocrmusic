#include "line_detection.hh"
#include "tools.hh"

#define MARGIN_SCAN 0.01
#define EPSILON 0.01

using namespace cv;

void neighbors(std::vector<Line>& mylines,
               cv::Mat& img)
{
  double ratio = 0.3;
  int marginerror = 1;

  for (size_t k = 0; k < mylines.size(); ++k)
  {
    int errcount = 0;

    while (1)
    {
      int energy = 0.0;

      int y = mylines[k].y + mylines[k].h;

      if ((y >= 0) && (y < img.size().height))
        for (int x = 0; x < img.size().width; ++x)
          energy += (int)img.at<uchar>(y, x) / 255;
      if (energy >= ratio * img.size().width)
      {
        errcount = 0;
        ++mylines[k].h;
      }
      else
        ++errcount;
      if (errcount >= marginerror)
        break;
    }
    errcount = 0;

    while (1)
    {
      int energy = 0.0;
      int y = mylines[k].y - 1;

      if ((y >= 0) && (y < img.size().height))
        for (int x = 0; x < img.size().width; ++x)
          energy += (int)img.at<uchar>(y, x) / 255;
      if ((energy >= ratio * img.size().width) && (mylines[k].y >= 1))
      {
        errcount = 0;
        --mylines[k].y;
        ++mylines[k].h;
      }
      else
        ++errcount;
      if (errcount >= marginerror)
        break;
    }
  }
}

std::vector<Line> detect_lines(cv::Mat& img,
                               double max_rot)
{
  std::vector<Line> mylines;
  cv::Mat ret(img.size(), CV_8UC1);
  double evalrot = 0.0;

  linedetection_preprocess(img, ret);
  evalrot = get_raw_lines(mylines, ret, max_rot, 0);
  filter_lines(mylines, img.size().height);

  if (mylines.size() == 0)
  {
    cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point(ret.size().width / 2,
                                                        ret.size().height / 2), 0.0, 1);

    std::cout << "Image may be rotated" << std::endl;
    cv::Mat rotated(ret.clone());
    for (double angle = -5.5; angle <= -5.0; angle += 0.1)
    {
      rot_mat = cv::getRotationMatrix2D(cv::Point(ret.size().width / 2,
                                                  ret.size().height / 2), angle, 1);
      cv::warpAffine(ret, rotated, rot_mat, ret.size(), cv::INTER_CUBIC);
      get_raw_lines(mylines, rotated, max_rot, 1);
      filter_lines(mylines, img.size().height);
      if (mylines.size() > 0)
      {
        break;
      }
    }

    cv::Mat dst(img.clone());
    cv::warpAffine(img, dst, rot_mat, img.size(),
                   cv::INTER_CUBIC,
                   BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
    img = dst;
  }
  neighbors(mylines, ret);

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

  mysort(lines, order_y);
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

  for (int i = 0; i < (int)lines.size() - 4; ++i)
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
  cv::threshold(img, img, 80.0, 255.0, cv::THRESH_BINARY);
}

double get_raw_lines(std::vector<Line>& mylines,
                     cv::Mat& img, double max_rot,
                     bool is_rot)
{
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
      if (energy >= 0.5 * img.size().width)
      {
        Line n;

        n.y = lines[i][0];
        n.h = 1;
        n.angle = is_rot;
        mylines.push_back(n);
      }
    }
  }

  return 0.0;
}

void display_lines(cv::Mat& img,
                   std::vector<Line>& mylines,
                   int rgb)
{
  int thick = 1;

  if ((mylines.size() > 0) && (mylines[0].angle))
    thick = 3;
  for (size_t i = 0; i < mylines.size(); ++i)
  {
    Vec4i l;

    for (size_t k = 0; k < mylines[i].h; ++k)
    {
      l[0] = 0;
      l[1] = mylines[i].y + k;
      l[2] = img.size().width;
      l[3] = mylines[i].y + k;
      line(img, Point(l[0], l[1]),
           Point(l[2], l[3]),
           Scalar(rgb & 0x0000ff, rgb & 0x00ff00, rgb & 0xff0000),
           thick,
           8);
    }
  }
}

