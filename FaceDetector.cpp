//
// Created by kuoppves on 21.8.2016.
//

#include <iostream>
#include "FaceDetector.h"

FaceDetector::FaceDetector(string datadir, string license, int dev) {
/*
    if (dev == 0) {
        crowdSight = new  CrowdSight(datadir, REDISTRIBUTION);
    } else {
        crowdSight = new CrowdSight(datadir, DEVELOPER);
    }

    if (!crowdSight->authenticate(license)) {
        delete crowdSight;
        crowdSight = NULL;
    }
*/
}

FaceDetector::FaceDetector(Parameters &parameters) {
/*
    if (parameters.dev == 0) {
        crowdSight = new  CrowdSight(parameters.datadir, REDISTRIBUTION);
    } else {
        crowdSight = new CrowdSight(parameters.datadir, DEVELOPER);
    }

    if (!crowdSight->authenticate(parameters.license)) {
        std::cerr << "Error:: Could not authenticate CrowdSight witj key: " << parameters.license << std::endl;
        std::cerr << crowdSight->getErrorDescription() << std::endl;
        delete crowdSight;
        crowdSight = NULL;
    }

    if (parameters.dev == 1) {
        std::cout << "Detector type: " << crowdSight->getFaceDetector() << std::endl;
    }
*/
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

    std::vector<Person> people;
    crowdSight->getCurrentPeople(people);

    std::vector<Person>::iterator it;


    for (int i = 0; i < people.size(); i++) {

        Person p = people[i];
        std::map<long, DetectedPerson>::iterator detIt;
        detIt = detectedPersonMap.find(p.getID());
        std::cout << detIt->second.getCrowdSightID() << std::endl;


        if (detIt != detectedPersonMap.end()) {
            detIt->second.incrementDetectionCount();
            detIt->second.addAge(p.getAge());
            detIt->second.addGender(p.getGender());
            detIt->second.setLastObserved(frameTimestamp);
            detIt->second.setLastObservedFrame(frameNo);

            if (this->isCloserTo(detIt->second.getGaze(), p.getHeadGaze(), cv::Point(paras.ad_point_x, paras.ad_point_y)))
                detIt->second.setGaze(p.getHeadGaze());

            if (detIt->second.getAttentionSpan() < p.getAttentionSpan())
                detIt->second.setAttentionSpan(p.getAttentionSpan());

            if (!detIt->second.getDetected()) {
                if (detIt->second.getDetectionCount() >= paras.detectionCount) {
                    detIt->second.setDetected(true);
                    detIt->second.setPersonId();
                    std::cout << "Detected as person " << detIt->second.getPersonId() << "frame " << frameNo << " gaze " << p.getHeadGaze().x << " "
                              << p.getHeadGaze().y << "(idCounter: " << DetectedPerson::idCounter << ")" << std::endl;


                } else if ( frameNo - detIt->second.getDetectionFrameNo() > paras.detectionCount) {
                    std::cout << "not accepted as person -> removing" << std::endl;
                    detectedPersonMap.erase(detIt);

                }
            }
        } else if (p.getID() != 0) {
            DetectedPerson newPerson;
            newPerson.setCrowdSightID(p.getID());
            newPerson.setDetectionCount(1);
            newPerson.setDetectionFrameNo(frameNo);
            newPerson.setLastObserved(frameTimestamp);
            newPerson.setFirstObservedTimestamp(frameTimestamp);
            newPerson.setGaze(p.getHeadGaze());

            detectedPersonMap[newPerson.getCrowdSightID()] = newPerson;

            std::cout << "First sight, person " << p.getID() << " frame: " << frameNo << " gaze: " << p.getHeadGaze().x << " " << p.getHeadGaze().y << std::endl;
        }
    }


}
/*
    for (it == people.begin(); it < people.end(); it++) {
        std::map<long, DetectedPerson>::iterator detIt;
        detIt = detectedPersonMap.find(it->getID());

        if (detIt != detectedPersonMap.end()) {
            detIt->second.incrementDetectionCount();
            detIt->second.addAge(it->getAge());
            detIt->second.addGender(it->getGender());
            detIt->second.setLastObserved(frameTimestamp);
            detIt->second.setLastObservedFrame(frameNo);

            if (this->isCloserTo(detIt->second.getGaze(), it->getHeadGaze(), cv::Point(paras.ad_point_x, paras.ad_point_y)))
                detIt->second.setGaze(it->getHeadGaze());

            if (detIt->second.getAttentionSpan() < it->getAttentionSpan())
                detIt->second.setAttentionSpan(it->getAttentionSpan());

            if (!detIt->second.getDetected()) {
                if (detIt->second.getDetectionCount() >= paras.detectionCount) {
                    detIt->second.setDetected(true);
                    detIt->second.setPersonId();
                    std::cout << "Detected as person " << detIt->second.getPersonId() << "frame " << frameNo << " gaze " << it->getHeadGaze().x << " "
                              << it->getHeadGaze().y << "(idCounter: " << DetectedPerson::idCounter << ")" << std::endl;


                } else if ( frameNo - detIt->second.getDetectionFrameNo() > paras.detectionCount) {
                    std::cout << "not accepted as person -> removing" << std::endl;
                    detectedPersonMap.erase(detIt);

                }
            }
        } else if (it->getID() != 0) {
            DetectedPerson newPerson;
            newPerson.setCrowdSightID(it->getID());
            newPerson.setDetectionCount(1);
            newPerson.setDetectionFrameNo(frameNo);
            newPerson.setLastObserved(frameTimestamp);
            newPerson.setFirstObservedTimestamp(frameTimestamp);
            newPerson.setGaze(it->getHeadGaze());

            detectedPersonMap[newPerson.getCrowdSightID()] = newPerson;

            std::cout << "First sight, person " << it->getID() << " frame: " << frameNo << " gaze: " << it->getHeadGaze().x << " " << it->getHeadGaze().y << std::endl;
        }
    }

}
*/
