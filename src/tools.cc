#include "tools.hh"

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
               std::vector<cv::Point>& points)
{
  std::queue<cv::Point> f;
  uchar pointedPixel = img.at<uchar>(y, x);
  bool visited[img.size().width][img.size().height];

  for (int i = 0; i < img.size().width; ++i)
    for (int j = 0; j < img.size().height; ++j)
      visited[i][j] = false;

  f.push(cv::Point(x, y));
  while (!f.empty())
  {
    cv::Point p = f.front();

    f.pop();
    if ((p.x >= 0) && (p.x < img.size().width)
        && (p.y >= 0) && (p.y < img.size().height)
        && (!visited[p.x][p.y]))
    {
      if (img.at<uchar>(p.y, p.x) == pointedPixel)
      {
        points.push_back(cv::Point(p.x, p.y));
        f.push(cv::Point(p.x - 1, p.y));
        f.push(cv::Point(p.x + 1, p.y));
        f.push(cv::Point(p.x, p.y - 1));
        f.push(cv::Point(p.x, p.y + 1));
      }
      visited[p.x][p.y] = true;
    }
  }
}

std::vector<cv::Rect> get_bounding_box(cv::Mat& ret)
{
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::Mat cop(ret.clone());

  findContours(cop, contours, hierarchy,
               CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  std::vector<cv::Rect> boundRect;
  std::vector<std::vector<cv::Point> > contours_poly(contours.size());

  for (size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);

    std::vector<cv::Point> tmp;

    for (size_t k = 0; k < contours_poly[i].size(); ++k)
      if (255 == ret.at<uchar>(contours_poly[i][k]))
	magicwand(ret, contours_poly[i][k].x,
		  contours_poly[i][k].y, tmp);
    boundRect.push_back(cv::boundingRect(cv::Mat(tmp)));
  }

  return boundRect;
}
