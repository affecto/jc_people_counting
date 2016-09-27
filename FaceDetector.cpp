//
// Created by kuoppves on 21.8.2016.
//

#include <iostream>
#include "FaceDetector.h"

FaceDetector::FaceDetector(Parameters &parameters) {

    if (parameters.dev == 0) {
        crowdSight = new CrowdSight(parameters.datadir, REDISTRIBUTION);
    } else {
        crowdSight = new CrowdSight(parameters.datadir, DEVELOPER);
    }

    if (!crowdSight->authenticate(parameters.license)) {
        std::cerr << "Error:: Could not authenticate CrowdSight with key: " << parameters.license << std::endl;
        std::cerr << crowdSight->getErrorDescription() << std::endl;
        delete crowdSight;
        crowdSight = NULL;
    }

    if (parameters.dev == 1) {
        std::cout << "Detector type: " << crowdSight->getFaceDetector() << std::endl;
    }

}

void FaceDetector::ShowSettings() {

    // Show CrowdSight settings
    cout << "Detection scale: " << crowdSight->getDetectionScale() << endl;
    cout << "Face confidence: " << crowdSight->getFaceConfidence() << endl;
    cout << "Face detector: " << crowdSight->getFaceDetector() << endl;
    cout << "Max Face Size: " << crowdSight->getMaxFaceSize() << endl;
    cout << "Min Face Size: " << crowdSight->getMinFaceSize() << endl;
    cout << "Is age used: " <<  crowdSight->isAgeUsed() << endl;
    cout << "Fast Detection: " << crowdSight->isFastDetectionUsed() << endl;
    cout << "Gender used: " <<  crowdSight->isGenderUsed() << endl;
    cout << "Head Pose Used: " << crowdSight->isHeadPoseUsed() << endl;
    cout << "Tracking used: " << crowdSight->isTrackingUsed()  << endl;
    cout << "Clothing Color Used: " << crowdSight->isClothingColorsUsed() << endl;
    cout << "Emotion Used: " << crowdSight->isEmotionsUsed() << endl;

}

void FaceDetector::DrawResults(cv::Mat frame, Person &person, int frameno) {
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
        time_t videoTime = (1.0*frameNo) / paras.desired_frame_rate;
        frameTimestamp = paras.startSecond + videoTime;
    }

    if (!crowdSight->process(frame)) {
        std::cerr << "Failed to process frame: " << crowdSight->getErrorDescription() << std::endl;
        return false;
    }

    std::vector<Person> persons;
    crowdSight->getCurrentPeople(persons);
    std::vector<Person>::iterator it;

    for (it = persons.begin(); it < persons.end(); it++) {
        std::map<long, DetectedPerson>::iterator detIt;
        detIt = detectedPersonMap.find(it->getID());

        if (paras.display != 0)
            DrawResults(frame, *it, frameNo);

        if (detIt != detectedPersonMap.end()) {
            detIt->second.incrementDetectionCount();
            detIt->second.addAge(it->getAge());
            detIt->second.addGender(it->getGender());
            detIt->second.setLastObserved(frameTimestamp);
            detIt->second.setLastObservedFrame(frameNo);
            detIt->second.setHeadYaw(it->getHeadYaw());
            detIt->second.setPossibilityToSee();

            if (isCloserTo(detIt->second.getGaze(), it->getHeadGaze(), cv::Point(paras.ad_point_x, paras.ad_point_y)))
                detIt->second.setGaze(it->getHeadGaze());

            if (detIt->second.getAttentionSpan() < it->getAttentionSpan())
                detIt->second.setAttentionSpan(it->getAttentionSpan());

            if (!detIt->second.getDetected()) {
                if (detIt->second.getDetectionCount() >= paras.detectionCount) {
                    detIt->second.setDetected(true);
                    detIt->second.setPersonId();
                    /*std::cout << "Detected as person " << detIt->second.getPersonId() << "frame " << frameNo <<
                            " gaze " << it->getHeadGaze().x << " " << it->getHeadGaze().y << "(idCounter: " <<
                            DetectedPerson::idCounter << ", possi. to see: " << detIt->second.getPossibilityToSee()
                            << ")" << std::endl;
                    */

                } else if ( frameNo - detIt->second.getDetectionFrameNo() > paras.detectionCount) {
                    //std::cout << "not accepted as person -> removing" << std::endl;
                    detectedPersonMap.erase(detIt);
                }
            }

            std::cout << "Observed person " << it->getID() << " frame " << frameNo
            << " attention " << it->getAttentionSpan() <<" gaze " << it->getHeadGaze().x << " " << it->getHeadGaze().y
            << " position" << it->getRightEye() << " head Yaw (degree) "<< detIt->second.getHeadYaw() << ", "
            << "possi. to see: " << detIt->second.getPossibilityToSee()<< std::endl;

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

            std::cout << "First sight, person " << it->getID() << " frame: " << frameNo << " gaze: " << it->getHeadGaze().x << " "
            << it->getHeadGaze().y << ", possi. to see: " << newPerson.getPossibilityToSee() << std::endl;
        }

    }

}

