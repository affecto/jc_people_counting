//
// Created by kuoppves on 20.8.2016.
//

#include "PeopleCountDetector.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

PeopleCountDetector::PeopleCountDetector(const Parameters& params) {

    uchar kernel_data[7*9] = {0,0,0,1,0,0,0,
    0,0,0,1,0,0,0,
    0,0,1,1,1,0,0,
    0,1,1,1,1,1,0,
    1,1,1,1,1,1,1,
    0,1,1,1,1,1,0,
    0,0,1,1,1,0,0,
    0,0,0,1,0,0,0,
    0,0,0,1,0,0,0};

    element = cv::Mat(7,9, CV_8U, kernel_data);

    line_det1 = new LineDetector("det1", 0.1, 0.5, 0.1, 1.0, params.frame_width, params.frame_height, element, params.scale_factor);
    line_det2 = new LineDetector("det2", 0.9, 0.5, 0.9, 1.0, params.frame_width, params.frame_height, element, params.scale_factor);

}

PeopleCountDetector::~PeopleCountDetector() {

    if (line_det1 != NULL) {
        delete line_det1;
        line_det1 = NULL;
    }

    if (line_det2 != NULL) {
        delete line_det2;
        line_det2 = NULL;
    }

}
