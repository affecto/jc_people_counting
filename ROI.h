//
// Created by kuoppves on 20.8.2016.
//

#ifndef JC_PILOT_ROI_H
#define JC_PILOT_ROI_H


#include <opencv2/opencv.hpp>

class ROI {

private:

    double scaled_center_x;
    double scaled_center_y;
    double scaled_width;
    double scaled_height;

public:

    double relative_center_x{0.5};
    double relative_center_y{0.5};
    double relative_width{0.5};
    double relative_height{0.5};
    double frame_width{512};
    double frame_height{512};

    ROI(double r_center_x, double r_center_y, double r_width, double r_height, const cv::Mat& frame);
    ROI(double r_center_x, double r_center_y, double r_width, double r_height, int frame_width, int frame_height);
    cv::Size getSize();
    cv::Point2d getCenter();

};


#endif //JC_PILOT_ROI_H
