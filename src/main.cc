#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <algorithm>

using namespace cv;

// Not that x1 and x2 may not be accurated
struct Line
{
  size_t x1;
  size_t x2;
  size_t y1;
  size_t y2;
};

void display(cv::Mat& img,
             double target_display_w);

bool sort_line(Vec4i i, Vec4i j)
{
  return (i[1] < j[1]);
}

std::vector<Vec4i> detect_lines(cv::Mat& img)
{
  std::vector<Vec4i> lines;
  std::vector<Vec4i> ret;

  HoughLinesP(img, lines, 1, M_PI / 2.0, 50.0, 10.0);
  for (size_t i = 0; i < lines.size(); ++i)
  {
    if ((fabs(lines[i][1] - lines[i][3]) < 10) &&
        (fabs(lines[i][0] - lines[i][2]) > img.size().width * 0.1))
      ret.push_back(lines[i]);
  }
  sort(ret.begin(), ret.end(), sort_line);

  std::vector<Line> mylines;
  int lasty = -1;
  int lasty2 = -1;
  // L'idee est la mais le code est naze et faux
  for (size_t i = 0; i < ret.size(); ++i)
  {
    if ((lasty < 0) ||
        ((fabs(lasty - ret[i][1]) > 2) &&
         (fabs(lasty2 - ret[i][3]) > 2)))
    {
      Line n;

      if (ret[i][0] < ret[i][2])
      {
        n.x1 = ret[i][0];
        n.x2 = ret[i][2];
        n.y1 = ret[i][1];
        n.y2 = ret[i][3];
      }
      else
      {
        n.x1 = ret[i][2];
        n.x2 = ret[i][0];
        n.y1 = ret[i][3];
        n.y2 = ret[i][1];
      }
      mylines.push_back(n);
    }
    else
    {
      mylines[mylines.size() - 1].y2 = ret[i][3];
      mylines[mylines.size() - 1].x1 = ret[i][3];
    }
    lasty = ret[i][1];
    lasty2 = ret[i][3];
  }

  ret.clear();
  for (size_t i = 0; i < mylines.size(); ++i)
  {
    Vec4i l;

    l[0] = mylines[i].x1;
    l[2] = mylines[i].x2;
    /*
      if (mylines[i].x1 < mylines[i].x2)
      {
      l[0] = 0;
      l[2] = img.size().width;
      }
      else
      {
      l[0] = img.size().width;
      l[2] = 0;
      }
    */

    l[1] = mylines[i].y1;
    l[3] = mylines[i].y2;
    //std::cout << mylines[i].x1 << " " <<  mylines[i].x2 << std::endl;
    ret.push_back(l);
  }
  return ret;
}

void remove_lines(cv::Mat& img,
                  std::vector<Vec4i>& lines)
{
  Scalar sc;

  sc[0] = 0;
  cv::Mat mask(img.size(), CV_8UC1, sc);

  for( size_t i = 0; i < lines.size(); i++ )
    line(mask, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(255), 1, 8);

  for (int y = 0; y < img.size().height; ++y)
    for (int x = 0; x < img.size().width; ++x)
      if (mask.at<uchar>(y, x) > 0)
      {
        bool something = 0;

        if ((y >= 1) && (y + 1 < img.size().height))
        {
          something |= (img.at<uchar>(y - 1, x) > 0) && (img.at<uchar>(y + 1, x) > 0);
          if ((x >= 1) && (x + 1 < img.size().width))
          {
            something |= (img.at<uchar>(y - 1, x - 1) > 0) && (img.at<uchar>(y + 1, x + 1) > 0);
            something |= (img.at<uchar>(y - 1, x + 1) > 0) && (img.at<uchar>(y + 1, x - 1) > 0);
          }
        }
        if (!something)
          img.at<uchar>(y, x) = 0;
      }
}

void display_lines(cv::Mat& img,
                   std::vector<Vec4i>& lines)
{
  for( size_t i = 0; i < lines.size(); i++ )
  {
    line(img, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
  }
}

void display(cv::Mat& img,
             double target_display_w)
{
  Mat display(Size(target_display_w,
                   (target_display_w / (double)img.size().width)* img.size().height),
              CV_8UC1);
  resize(img, display, display.size(), 0, 0, INTER_LINEAR);
  namedWindow("Skin");
  imshow("Skin", display);
  waitKey(0);
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: "
              << argv[0]
              << " sheet_image"
              << std::endl;
  }
  else
  {
    cv::Mat img = cv::imread(argv[1]);
    double target_width = img.size().width;
    double target_height = (target_width / (double)img.size().width)* img.size().height;
    Mat dst(Size(target_width, target_height), CV_8UC1);
    Mat dst2(Size(target_width, target_height), CV_8UC1);
    std::vector<Vec4i> lines;

    resize(img, dst, dst.size(), 0, 0, INTER_LINEAR);
    cvtColor(dst, dst2, CV_RGB2GRAY);
    dst = dst2;

    threshold(dst, dst, 200.0, 255.0, THRESH_BINARY_INV);
    lines = detect_lines(dst);
    std::cout << "Total lines = " << lines.size() << std::endl;
    display_lines(img, lines);

    display(img, 500);
    imwrite("output.png", img);

    //remove_lines(dst, lines);
    //imwrite("output_no_line.png", dst);
    //display(dst, 700);
  }
  return 0;
}
