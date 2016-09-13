//
// Created by kuoppves on 21.8.2016.
//

#ifndef JC_PILOT_FACEDETECTOR_H
#define JC_PILOT_FACEDETECTOR_H


#include <crowdsight.h>
#include "Parameters.h"
#include "DetectedPerson.h"

class FaceDetector {

public:

    time_t frameTimestamp;

    CrowdSight *crowdSight;
    std::map<long, DetectedPerson> detectedPersonMap;

    bool isCloserTo(cv::Point origPoint, cv::Point comparePoint, cv::Point adPoint);

    FaceDetector(Parameters& parameters);
    FaceDetector(string datadir, string license, int dev);

    bool Process(cv::Mat frame, Parameters& paras, int frameNo, bool fileSource);

    void ShowSettings();

};


#endif //JC_PILOT_FACEDETECTOR_H
