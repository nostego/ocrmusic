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

void display_rect(cv::Mat& img,
		  std::vector<cv::Rect>& boundRect)
{
  cv::Scalar sc;

  sc[0] = 255;
  sc[1] = 0;
  sc[2] = 0;

  for (size_t i = 0; i < boundRect.size(); i++)
  {
    cv::Point pt1;
    cv::Point pt2;

    pt1.x = boundRect[i].x;
    pt1.y = boundRect[i].y;
    pt2.x = boundRect[i].x + boundRect[i].width;
    pt2.y = boundRect[i].y + boundRect[i].height;

    cv::rectangle(img, pt1, pt2, sc);
  }
}

std::vector<cv::Rect> get_bounding_box(cv::Mat& ret)
{
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  findContours(ret, contours, hierarchy,
               CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

  std::vector<cv::Rect> boundRect(contours.size() );
  std::vector<std::vector<cv::Point> > contours_poly(contours.size());

  for (size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
    boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
  }
  bool del[boundRect.size()];

  for (size_t i = 0; i < boundRect.size(); i++)
    for (size_t j = 0; j < boundRect.size(); j++)
    {
      if ((i != j) && (boundRect[i].x >= boundRect[j].x) &&
          (boundRect[i].x + boundRect[i].width
           <= boundRect[j].x + boundRect[j].width) &&
          (boundRect[i].y >= boundRect[j].y) &&
          (boundRect[i].y + boundRect[i].height
           <= boundRect[j].y + boundRect[j].height))
        del[i] = true;
    }
  filter(boundRect, del);
  return boundRect;
}
