//
// Created by kuoppves on 21.8.2016.
//

#include <iostream>
#include "FaceDetector.h"
extern el::Logger* mainLogger;

FaceDetector::FaceDetector(Parameters &parameters) {
    if (parameters.getLicenseMode() == 0) {
        crowdSight = new CrowdSight(parameters.getdataDir(), REDISTRIBUTION);
    } else {
        crowdSight = new CrowdSight(parameters.getdataDir(), DEVELOPER);
    }

    update_parameters(parameters.getconfiguration());

    if (!crowdSight->authenticate(parameters.getlicense())) {
        mainLogger->error("Error:: Could not authenticate CrowdSight with key: %v", parameters.getlicense());
        mainLogger->error("%v", crowdSight->getErrorDescription());
        delete crowdSight;
        crowdSight = NULL;
    }

    if (parameters.getLicenseMode() == 1) {
        mainLogger->info("Detector type: %v", crowdSight->getFaceDetector());
    }

}

void FaceDetector::ShowSettings() {

    // Show CrowdSight settings
    mainLogger->info("Detection scale: %v", crowdSight->getDetectionScale());
    mainLogger->info("Face confidence: %v", crowdSight->getFaceConfidence());
    mainLogger->info("Face detector: %v", crowdSight->getFaceDetector());
    mainLogger->info("Min, max Face Size: (%v, %v)", crowdSight->getMinFaceSize(), crowdSight->getMaxFaceSize());
    mainLogger->info("Fast Detection: %v", crowdSight->isFastDetectionUsed());
    mainLogger->info("Is age used: %v", crowdSight->isAgeUsed());
    mainLogger->info("Gender used: %v", crowdSight->isGenderUsed());
    mainLogger->info("Head pose used: %v", crowdSight->isHeadPoseUsed());
    mainLogger->info("Tracking used: %v", crowdSight->isTrackingUsed());
    mainLogger->info("Clothing color used: %v", crowdSight->isClothingColorsUsed());
    mainLogger->info("emotion used: %v", crowdSight->isEmotionsUsed());

}

void FaceDetector::DrawResults(cv::Mat frame, Person &person) {
    std::map<long, DetectedPerson>::iterator detIt;
    detIt = detectedPersonMap.find(person.getID());
    std::string yaw = "yaw: ";
    std::string poss = "psb. to see: ";
    std::string yaw_degree = yaw + std::to_string(detIt->second.getHeadYaw());
    std::string possibility_to_see = poss + detIt->second.getPossibilityToSee();

    cv::Point rightEyePosition = person.getRightEye();
    cv::Point leftEyePosition = person.getLeftEye();

    cv::circle(frame, rightEyePosition, 3, vcolor::KCOLOR_RED_1);
    cv::circle(frame, leftEyePosition, 3, vcolor::KCOLOR_RED_1);
    cv::rectangle(frame, person.getFaceRect(), vcolor::KCOLOR_RED_1);
    std::string idstring = std::to_string(person.getID());
    cv::putText( frame, idstring,  cv::Point(person.getFaceRect().x, person.getFaceRect().y)
            , cv::FONT_HERSHEY_PLAIN, 1, vcolor::KCOLOR_GREEN_1 );
    std::string gazeString = std::to_string(person.getHeadGaze().x) + std::to_string(person.getHeadGaze().y);
    cv::putText(frame, gazeString,  cv::Point(person.getFaceRect().x + 15, person.getFaceRect().y)
            , cv::FONT_HERSHEY_PLAIN, 1, vcolor::KCOLOR_GREEN_1 );

    cv::putText(frame, possibility_to_see, cv::Point(person.getFaceRect().x, person.getFaceRect().y - 15),
                                                     cv::FONT_HERSHEY_PLAIN, 1, vcolor::KCOLOR_BLACK_1 );
    cv::putText(frame, yaw_degree, cv::Point(person.getFaceRect().x, person.getFaceRect().y - 25),
                                                     cv::FONT_HERSHEY_PLAIN, 1, vcolor::KCOLOR_BLACK_1 );

}

bool FaceDetector::isCloserTo(cv::Point origPoint, cv::Point comparePoint, cv::Point adPoint) {

    float distanceOrig = (adPoint.x - origPoint.x)^2 + (adPoint.y - origPoint.y)^2;
    float distanceComp = (adPoint.x - comparePoint.x)^2 + (adPoint.y - comparePoint.y)^2;
    return distanceComp < distanceOrig;
}

bool FaceDetector::Process(cv::Mat frame, Parameters &paras, int frameNo, bool fileSource) {

    frameTimestamp = time(0);

    if (fileSource) {
        time_t videoTime = (1.0*frameNo) / paras.getdesired_frame_rate();
        frameTimestamp = paras.getstartSecond() + videoTime;
    }

    if (!crowdSight->process(frame, false)) {
        std::cerr << "Failed to process frame: " << crowdSight->getErrorDescription() << std::endl;
        return false;
    }

    std::vector<Person> persons;
    crowdSight->getCurrentPeople(persons);
    std::vector<Person>::iterator it;

    for (it = persons.begin(); it < persons.end(); it++) {
        std::map<long, DetectedPerson>::iterator detIt;
        detIt = detectedPersonMap.find(it->getID());

        if (detIt != detectedPersonMap.end()) {
            detIt->second.incrementDetectionCount();
            detIt->second.addAge(it->getAge());
            detIt->second.addGender(it->getGender());
            detIt->second.setLastObserved(frameTimestamp);
            detIt->second.setLastObservedFrame(frameNo);
            detIt->second.setHeadYaw(it->getHeadYaw());
            detIt->second.setPossibilityToSee();

            if (paras.getis_display() != 0)
                DrawResults(frame, *it);

            if (isCloserTo(detIt->second.getGaze(), it->getHeadGaze(), cv::Point(paras.getad_point_x(), paras.getad_point_y())))
                detIt->second.setGaze(it->getHeadGaze());

            if (detIt->second.getAttentionSpan() < it->getAttentionSpan())
                detIt->second.setAttentionSpan(it->getAttentionSpan());

            if (!detIt->second.getDetected()) {
                if (detIt->second.getDetectionCount() >= paras.getdetectionCount()) {
                    detIt->second.setDetected(true);
                    detIt->second.setPersonId();
                } else if ( frameNo - detIt->second.getDetectionFrameNo() > paras.getdetectionCount()) {
                    detectedPersonMap.erase(detIt);
                }
            }
            mainLogger->info("[frame %v] Observed person: %v, age: %v, gender: %v, attention: %v, gaze: (%v, %v), position: [%v, %v], "
                                     "headYaw (degree): %v, possi. to see: %v",
                             frameNo, it->getID(), it->getAge(), it->getGender(), it->getAttentionSpan(), it->getHeadGaze().x, it->getHeadGaze().y,
                             float(it->getRightEye().x) / frame.cols, float(it->getRightEye().y) / frame.rows, detIt->second.getHeadYaw(), detIt->second.getPossibilityToSee());

        } else if (it->getID() != 0) {
            DetectedPerson newPerson;
            newPerson.setCrowdSightID(it->getID());
            newPerson.setDetectionCount(1);
            newPerson.setDetectionFrameNo(frameNo);
            newPerson.setLastObserved(frameTimestamp);
            newPerson.setFirstObservedTimestamp(frameTimestamp);
            newPerson.setGaze(it->getHeadGaze());
            newPerson.setHeadYaw(it->getHeadYaw());
            newPerson.setPossibilityToSee();

            detectedPersonMap[newPerson.getCrowdSightID()] = newPerson;

            mainLogger->info("[frame %v] First sight of the person: %v, age: %v, gender: %v, gaze: (%v, %v), position: [%v, %v], "
                                     "headYaw (degree): %v, possi. to see: %v",
                             frameNo, it->getID(), it->getAge(), it->getGender(), it->getHeadGaze().x, it->getHeadGaze().y,
                             float(it->getRightEye().x) / frame.cols, float(it->getRightEye().y) / frame.rows, it->getHeadYaw(), newPerson.getPossibilityToSee());
        }

    }

}


