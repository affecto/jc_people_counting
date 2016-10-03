//
// Created by kuoppves on 20.8.2016.
//

#include "PreProcessor.h"

#include <iostream>
#include "PreProcessor.h"

using namespace std;

PreProcessor::PreProcessor(int grayscale, double s_width, double s_height) {

    scale_width = s_width;
    scale_height = s_height;
    convert_to_grayscale = grayscale;
    //use_roi = u_roi;
    //roi = new ROI(r.relative_center_x, r.relative_center_y, r.relative_width, r.relative_height, r.frame_width, r.frame_height);

}

PreProcessor::PreProcessor(Parameters& paras) {

    scale_width = paras.getscale_factor() * paras.getframe_width();
    scale_height = paras.getscale_factor() * paras.getframe_height();
    convert_to_grayscale = paras.getuse_grayscale();
    //use_roi = paras.use_roi;
    //roi = new ROI(paras.roi_center_x, paras.roi_center_y, paras.roi_width, paras.roi_height, paras.frame_width, paras.frame_height);
}

cv::Mat PreProcessor::Process(const cv::Mat& frame) {

    if (convert_to_grayscale == 1) {
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
        cv::resize(gray_frame, resized_frame, cv::Size(scale_width, scale_height), cv::INTER_AREA);
    } else {
        cv::resize(frame, resized_frame, cv::Size(scale_width, scale_height), cv::INTER_AREA);
    }

    return resized_frame;

}