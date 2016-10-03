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

public:
    int convert_to_grayscale;

    double scale_width;
    double scale_height;

    PreProcessor(int grayscale, double s_width, double s_height);
    PreProcessor(Parameters& paras);

    cv::Mat Process(const cv::Mat& frame);
};

#endif //JC_PILOT_PREPROCESSOR_H
