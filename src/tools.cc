#include "tools.hh"

bool isvalid(cv::Mat& img,
             int x,
             int y)
{
  return ((x >= 0) &&
          (y >= 0) &&
          (x < img.size().width) &&
          (y < img.size().height));
}

void myerode(cv::Mat& img,
             cv::Mat& ret,
             cv::Size si)
{
  ret = img.clone();
  for (int y = 0; y < img.size().height; ++y)
    for (int x = 0; x < img.size().width; ++x)
    {
      bool okay = img.at<uchar>(y, x) / 255;

      for (int dy = y - si.height; dy < y + si.height; ++dy)
        for (int dx = x - si.width; dx < x + si.width; ++dx)
          if (((dx != x) || (dy != y)) && isvalid(img, dx, dy))
            okay &= img.at<uchar>(dy, dx) / 255;
      if (okay)
        ret.at<uchar>(y, x) = 255;
      else
        ret.at<uchar>(y, x) = 0;
    }
}


void mydilate(cv::Mat& img,
	      cv::Mat& ret,
	      cv::Size si)
{
  ret = img.clone();
  for (int y = 0; y < img.size().height; ++y)
    for (int x = 0; x < img.size().width; ++x)
    {
      bool okay = img.at<uchar>(y, x) / 255;

      for (int dy = y - si.height; dy < y + si.height; ++dy)
        for (int dx = x - si.width; dx < x + si.width; ++dx)
          if (((dx != x) || (dy != y)) && isvalid(img, dx, dy))
            okay |= img.at<uchar>(dy, dx) / 255;
      if (okay)
        ret.at<uchar>(y, x) = 255;
      else
        ret.at<uchar>(y, x) = 0;
    }
}

void display(cv::Mat& img,
             double target_display_w)
{
  cv::namedWindow("Display");
  if (target_display_w < 1)
    cv::imshow("Display", img);
  else
  {
    cv::Mat disp(cv::Size(target_display_w,
                          (target_display_w / (double)img.size().width)* img.size().height),
                 CV_8UC1);
    cv::resize(img, disp, disp.size(), 0, 0, cv::INTER_LINEAR);
    cv::imshow("Display", disp);
  }
  cv::waitKey(0);
}

bool collide(cv::Rect& a, cv::Rect& b)
{
  return !((a.y + a.height < b.y) ||
           (a.y > b.y + b.height) ||
           (a.x > b.x + b.width) ||
           (a.x + a.width < b.x));
}

void display_rect(cv::Mat& img,
                  std::vector<cv::Rect>& boundRect,
                  int rgb)
{
  cv::Scalar sc;

  sc[0] = rgb & 0x0000ff;
  sc[1] = rgb & 0x00ff00;
  sc[2] = rgb & 0xff0000;

  for (size_t i = 0; i < boundRect.size(); i++)
  {
    cv::Point pt1;
    cv::Point pt2;

    pt1.x = boundRect[i].x;
    pt1.y = boundRect[i].y;
    pt2.x = boundRect[i].x + boundRect[i].width;
    pt2.y = boundRect[i].y + boundRect[i].height;

    cv::rectangle(img, pt1, pt2, sc, 1 + img.size().height / 1000);
  }
}

void display_onerect(cv::Mat& img,
                     cv::Rect boundRect,
                     int rgb)
{
  cv::Scalar sc;

  sc[0] = rgb & 0x0000ff;
  sc[1] = rgb & 0x00ff00;
  sc[2] = rgb & 0xff0000;

  cv::Point pt1;
  cv::Point pt2;

  pt1.x = boundRect.x;
  pt1.y = boundRect.y;
  pt2.x = boundRect.x + boundRect.width;
  pt2.y = boundRect.y + boundRect.height;

  cv::rectangle(img, pt1, pt2, sc, 1 + img.size().height / 1000);
}

void magicwand(cv::Mat&img,
               int x,
               int y,
               std::vector<cv::Point>& points,
               bool* visited)
{
  std::queue<cv::Point> f;
  uchar pointedPixel = img.at<uchar>(y, x);

  f.push(cv::Point(x, y));
  while (!f.empty())
  {
    cv::Point p = f.front();

    f.pop();
    if ((p.x >= 0) && (p.x < img.size().width)
        && (p.y >= 0) && (p.y < img.size().height)
        && (!visited[p.x + p.y * img.size().width]))
    {
      if (img.at<uchar>(p.y, p.x) == pointedPixel)
      {
        points.push_back(cv::Point(p.x, p.y));
        f.push(cv::Point(p.x - 1, p.y));
        f.push(cv::Point(p.x + 1, p.y));
        f.push(cv::Point(p.x, p.y - 1));
        f.push(cv::Point(p.x, p.y + 1));
        f.push(cv::Point(p.x - 1, p.y - 1));
        f.push(cv::Point(p.x + 1, p.y - 1));
        f.push(cv::Point(p.x - 1, p.y + 1));
        f.push(cv::Point(p.x + 1, p.y + 1));
      }
      visited[p.x + p.y * img.size().width] = true;
    }
  }
}

std::vector<cv::Rect> get_bounding_box(cv::Mat& ret)
{
  std::vector<cv::Rect> boundRect;
  bool* visited = new bool[ret.size().width * ret.size().height];
  std::vector<cv::Point> tmp;

  for (int k = 0; k < ret.size().width * ret.size().height; ++k)
    visited[k] = false;

  for (int y = 0; y < ret.size().height; ++y)
  {
    for (int x = 0; x < ret.size().width; ++x)
    {
      if ((!visited[x + y * ret.size().width]) &&
          (255 == ret.at<uchar>(y, x)))
      {
        tmp.clear();
        magicwand(ret, x, y, tmp, visited);
        boundRect.push_back(cv::boundingRect(cv::Mat(tmp)));
      }
    }
  }
  delete[] visited;
  return boundRect;
}
