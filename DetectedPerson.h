//
// Created by kuoppves on 21.8.2016.
//

#ifndef JC_PILOT_DETECTEDPERSON_H
#define JC_PILOT_DETECTEDPERSON_H

#include <iostream>
#include <string>
#include <ctime>
#include <opencv2/opencv.hpp>
#include "MovingAvgVal.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"

class DetectedPerson {

    MovingAvgVal age;
    MovingAvgVal gender;
    int detectionCount;
    bool detected = false;
    int detectionFrameNo = -1;
    int lastObservedFrame = -1;
    time_t firstObservedTimestamp;
    time_t lastObserved;

    bool sent = false;
    cv::Point gaze;
    int crowdSightID;
    int attentionSpan = 0;
    float headYaw;
    std::string possibilityToSee;

private:

    int personId;
    int pts_verylikely = 0;
    int pts_likely = 0;
    int pts_notlikely = 0;
    float yaw_verylikely = 0.0f;
    float yaw_likely = 0.0f;
    float yaw_notlikely = 0.0f;

public:

    static int idCounter;

    time_t getLastObserved() const {
        return lastObserved;
    }

    void setLastObserved(time_t lObserved) {
        lastObserved = lObserved;
    }

    int getPersonId() const {
        return personId;
    }

    void setPersonId() {
        personId = idCounter++;

    }

    int getCrowdSightID() const {
        return crowdSightID;
    }

    void setCrowdSightID(int id) {
        crowdSightID = id;
    }

    float getGender() {
        gender.calcValue();
        return gender.getValue();
    }

    std::string getGenderString() {
        gender.calcValue();
        return ((gender.getValue()) < 0 ? "mies":"nainen");
    }

    float getAge() {
        age.calcValue();
        return age.getValue();
    }

    void addAge(int a) {
        age.addValue(a);
    }

    void addGender(float g) {
        gender.addValue(g);
    }

    void incrementDetectionCount() {
        detectionCount++;
    }

    int getDetectionCount() const {
        return detectionCount;
    }

    void setDetectionCount(int dCount) {
        detectionCount = dCount;
    }

    bool getDetected() const {
        return detected;
    }

    void setDetected(bool d) {
        detected = d;
    }

    time_t getFirstObservedTimeStamp() const {
        return firstObservedTimestamp;
    }

    void setFirstObservedTimestamp(time_t dTimestamp) {
        firstObservedTimestamp = dTimestamp;
    }

    bool isSent() const {
        return sent;
    }

    void setSent(bool s) {
        sent = s;
    }

    const cv::Point &getGaze() const {
        return gaze;
    }

    void setGaze(const cv::Point &g) {
        gaze = g;
    }

    double getHeadYaw() {
        return headYaw;
    }

    void setHeadYaw(float yaw) {
        headYaw  = (yaw * 180.0) / M_PI;
    }

    std::string getPossibilityToSee() {
        return possibilityToSee;
    }

    int categorizeHeadYaw(float degrees) {

        if ( -3.0 <= degrees && degrees <= 3.0 ) {
            return 1;
        }

        if ( -6.0 <= degrees && degrees <= 6.0 ) {
            return 2;
        }

        return 3;

    }

    void setPossibilityToSee() {
        int category = categorizeHeadYaw(headYaw);
        increment_pts_and_yaw(category);

        if (category == 1) {
            possibilityToSee = "Very likely";
        }
        else if (category == 2) {
            possibilityToSee = "Likely";
        }
        else {
            possibilityToSee = "Not likely";
        }
    }

    // needed for making up dummy person
    void setPossibilityToSee(std::string possibilityToSee_) {
        possibilityToSee = possibilityToSee_;
    }

    void setReportedPossibilityToSee() {
        if (pts_verylikely > 0) {
            setPossibilityToSee("Very likely");
            setReportedheadYaw(yaw_verylikely);
        }
        else if (pts_likely > 0) {
            setPossibilityToSee("Likely");
            setReportedheadYaw(yaw_likely);
        }
        else {
            setPossibilityToSee("Not likely");
            setReportedheadYaw(yaw_notlikely);
        }
    }

    void setReportedheadYaw(float average_headYaw_byCategory) {
        headYaw = average_headYaw_byCategory;
    }

    void increment_pts_and_yaw(int category) {
        if (category == 1) {
            pts_verylikely += 1;
            yaw_verylikely += headYaw;
        }
        else if (category == 2) {
            pts_likely += 1;
            yaw_likely += headYaw;
        }
        else {
            pts_notlikely += 1;
            yaw_notlikely += headYaw;
        }
    }

    void average_yaw_byCategory() {
        if (pts_verylikely != 0)
            yaw_verylikely /= pts_verylikely;
        if (pts_likely != 0)
            yaw_likely /= pts_likely;
        if (pts_notlikely != 0)
            yaw_notlikely /= pts_notlikely;
    }

    int getDetectionFrameNo() const {
        return detectionFrameNo;
    }

    void setDetectionFrameNo(int dFrameNo) {
        detectionFrameNo = dFrameNo;
    }

    int getAttentionSpan() const {
        return attentionSpan;
    }

    void setAttentionSpan(int aSpan) {
        attentionSpan = aSpan;
    }

    void setLastObservedFrame(int i) {
        lastObservedFrame = i;
    }

    int getLastObservedFrame() const {
        return lastObservedFrame;
    }

    std::string asJSON(int frameID, int unitID, int countedPedestrians);


};


#endif //JC_PILOT_DETECTEDPERSON_H
