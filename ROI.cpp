//
// Created by kuoppves on 20.8.2016.
//

#include "ROI.h"

#include "ROI.h"

ROI::ROI(double r_center_x, double r_center_y, double r_width, double r_height,
         const cv::Mat &frame) {

    relative_center_x = r_center_x;
    relative_center_y = r_center_y;
    relative_width = r_width;
    relative_height = r_height;
    scaled_center_x = frame.cols * relative_center_x;
    scaled_center_y = frame.rows * relative_center_y;
    scaled_width = frame.cols*relative_width;
    scaled_height = frame.rows*relative_height;
    frame_width = frame.cols;
    frame_height = frame.rows;
}

ROI::ROI(double r_center_x, double r_center_y, double r_width, double r_height, int f_width, int f_height) {

    relative_center_x = r_center_x;
    relative_center_y = r_center_y;
    relative_width = r_width;
    relative_height = r_height;

    scaled_center_x = f_width * r_center_x;
    scaled_center_y = f_height * r_center_y;
    scaled_width = f_width * r_width;
    scaled_height = f_height * r_height;
    frame_width = f_width;
    frame_height = f_height;

}

cv::Size ROI::getSize() {
    cv::Size size(scaled_width, scaled_height);
    return size;

}

cv::Point2d ROI::getCenter() {
    cv::Point2d center(scaled_center_x, scaled_center_y);
    return center;
}