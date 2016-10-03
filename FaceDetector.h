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

private:

    void update_parameters(rapidjson::Document &configuration) {
        if (configuration.HasMember("cs_acceptance_threshold"))
            crowdSight->setFaceConfidence(configuration["cs_acceptance_threshold"].GetDouble());
        if (configuration.HasMember("cs_min_face_size"))
            crowdSight->setMinFaceSize(configuration["cs_min_face_size"].GetDouble());
        if (configuration.HasMember("cs_max_face_size"))
            crowdSight->setMaxFaceSize(configuration["cs_max_face_size"].GetDouble());
        if (configuration.HasMember("cs_use_age"))
            crowdSight->useAge(configuration["cs_use_age"].GetBool());
        if (configuration.HasMember("cs_use_cloth_color"))
            crowdSight->useClothColors(configuration["cs_use_cloth_color"].GetBool());
        if (configuration.HasMember("cs_use_emotions"))
            crowdSight->useEmotions(configuration["cs_use_emotions"].GetBool());
        if (configuration.HasMember("cs_use_ethnicity"))
            crowdSight->useEthnicity(configuration["cs_use_ethnicity"].GetBool());
        if (configuration.HasMember("cs_use_gender"))
            crowdSight->useGender(configuration["cs_use_gender"].GetBool());
        if (configuration.HasMember("cs_use_head_pose"))
            crowdSight->useHeadPose(configuration["cs_use_head_pose"].GetBool());
        if (configuration.HasMember("cs_use_mood"))
            crowdSight->useMood(configuration["cs_use_mood"].GetBool());
        if (configuration.HasMember("cs_use_fastdetection"))
            crowdSight->useFastDetection(configuration["cs_use_fastdetection"].GetBool());




    }

};


#endif //JC_PILOT_FACEDETECTOR_H
