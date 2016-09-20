//
// Created by kuoppves on 21.8.2016.
//

#ifndef JC_PILOT_FACEDETECTOR_H
#define JC_PILOT_FACEDETECTOR_H


#include <crowdsight.h>
#include "Parameters.h"
#include "DetectedPerson.h"
namespace VisualizationColor
{
    // Some color constants
    const cv::Scalar KCOLOR_GREEN_1 ( 0x10, 0xaa, 0x10 );
    const cv::Scalar KCOLOR_RED_1   ( 0x00, 0x00, 0xff );
    const cv::Scalar KCOLOR_BLACK_1 ( 0x00, 0x00, 0x00 );
}
namespace vcolor = VisualizationColor;

class FaceDetector {

public:

    time_t frameTimestamp;

    CrowdSight *crowdSight;
    std::map<long, DetectedPerson> detectedPersonMap;

    bool isCloserTo(cv::Point origPoint, cv::Point comparePoint, cv::Point adPoint);

    FaceDetector(Parameters& parameters);
    //FaceDetector(string datadir, string license, int dev);

    bool Process(cv::Mat frame, Parameters& paras, int frameNo, bool fileSource);

    void DrawResults(cv::Mat frame, Person &person, int frameno);
    void ShowSettings();

};


#endif //JC_PILOT_FACEDETECTOR_H
