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

    // params.roi_vlines[0:4]: upper left x, upper left y, bottom right x, bottom right y, e.g. 0.1 0.5 0.9 1.0
    // then it creates two virtual lines, one connects (0.1, 0.5) and (0.1, 1.0), another connects (0.9, 0.5) and (0.9, 1.0)
    line_det1 = new LineDetector("det1", params.roi_vlines[0], params.roi_vlines[1], params.roi_vlines[0], params.roi_vlines[3],
                                 params.frame_width, params.frame_height, element, params.scale_factor, params.blob_area_threshold);
    line_det2 = new LineDetector("det2", params.roi_vlines[2], params.roi_vlines[1], params.roi_vlines[2], params.roi_vlines[3],
                                 params.frame_width, params.frame_height, element, params.scale_factor, params.blob_area_threshold);

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
