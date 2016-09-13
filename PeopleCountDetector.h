//
// Created by kuoppves on 20.8.2016.
//

#ifndef JC_PILOT_PEOPLECOUNTDETECTOR_H
#define JC_PILOT_PEOPLECOUNTDETECTOR_H

#include <opencv2/opencv.hpp>
#include "Parameters.h"
#include "LineDetector.h"

class PeopleCountDetector {

public:

    cv::Mat element;
    LineDetector* line_det1;
    LineDetector* line_det2;


    PeopleCountDetector(const Parameters& params);
    ~PeopleCountDetector();

};


#endif //JC_PILOT_PEOPLECOUNTDETECTOR_H
