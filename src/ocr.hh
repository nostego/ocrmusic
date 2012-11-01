#ifndef OCR_HH
 #define OCR_HH

class Ocr
{
  public:
    Ocr(cv::Mat* img);
    void findGKeys();
    void findFKeys();
  private:
    cv::Mat* img_;
};

#endif
