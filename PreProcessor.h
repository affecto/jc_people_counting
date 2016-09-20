//
// Created by kuoppves on 20.8.2016.
//

#ifndef JC_PILOT_PREPROCESSOR_H
#define JC_PILOT_PREPROCESSOR_H


#include "ROI.h"
#include "Parameters.h"

class PreProcessor {
private:
    cv::Mat gray_frame;
    cv::Mat resized_frame;
    cv::Mat cropped_frame;

public:
    int convert_to_grayscale;
    bool use_roi;

    double scale_width;
    double scale_height;
    ROI* roi;


    PreProcessor(int grayscale, double s_width, double s_height, ROI r, bool u_roi);
    PreProcessor(const Parameters& paras);

    cv::Mat Resize();
    cv::Mat ConvertToGrayScale();
    cv::Mat ClipWithROI(ROI roi);
    cv::Mat Process(const cv::Mat& frame);
};

#endif //JC_PILOT_PREPROCESSOR_H
