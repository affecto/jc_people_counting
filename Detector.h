//
// Created by kuoppves on 20.8.2016.
//

#ifndef JC_PILOT_DETECTOR_H
#define JC_PILOT_DETECTOR_H

#include <string>
#include "Parameters.h"
#include <opencv2/opencv.hpp>
#include "PreProcessor.h"
#include "PeopleCountDetector.h"
#include "FaceDetector.h"

using namespace std;

class Detector {
public:
    Parameters *parameters;
    PreProcessor *preprocessor;
    int frameNo;
    bool fileSource;
    cv::VideoCapture videoCapture;
    const char* window_name;
    int skip_frames;

    cv::Mat preprocessed_frame;
    PeopleCountDetector *people_count_detector;
    FaceDetector *face_detector;
    int countedPedestrians;

    bool status;

    Detector(Parameters *param) : parameters(param) {}
    ~Detector() {
        if (parameters != 0) delete parameters;

    }

    bool init();
    bool ok();
    void InitPreProcessor();
    void InitOpenCV();
    void InitDetectors();

    void run();

    void sendJsonData(std::map<long, DetectedPerson> detectedPeopleMap, int countedPedestrians, int frameNo, Parameters *parameters);


};


#endif //JC_PILOT_DETECTOR_H
