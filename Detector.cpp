//
// Created by kuoppves on 20.8.2016.
//

#include "Detector.h"
#include <sys/time.h>
#include "include/rapidjson/document.h"
#include <restclient-cpp/restclient.h>
extern el::Logger* mainLogger;

DetectedPerson* createDummyProfile() {

    DetectedPerson *newPerson = new DetectedPerson();
    time_t now = time(NULL);
    newPerson->setCrowdSightID(-1);
    newPerson->setDetectionCount(0);
    newPerson->setDetectionFrameNo(-1);
    newPerson->setLastObserved(now);
    newPerson->setFirstObservedTimestamp(now);
    newPerson->setGaze(cv::Point(0,0));
    newPerson->setHeadYaw(0);
    newPerson->setPossibilityToSee("Not likely");
    newPerson->setAttentionSpan(0);

    return newPerson;
}

void Detector::sendJsonData(std::map<long, DetectedPerson> detectedPeopleMap, int countedPedestrians, int frameNo, Parameters *parameters) {

    mainLogger->info("preparing json string to be sent");

    RestClient::headermap headers;

    std::map<long, DetectedPerson>::iterator pIt;
    pIt = detectedPeopleMap.begin();
    int count = 0;
    while (pIt != detectedPeopleMap.end()) {
        if (pIt->second.getDetected())
            count++;
        pIt++;
    }

    vector<int> countedPedestrians_segs;
    if (count > 0) {
        for (int i = 0; i < count; i++)
            countedPedestrians_segs.push_back(0);

        if (countedPedestrians > count) {
            int seg_length = int(countedPedestrians / count);
            int i = 0;
            while (i < count) {
                if (i == count - 1)
                    countedPedestrians_segs[i] = countedPedestrians - seg_length * i;
                else
                    countedPedestrians_segs[i] = seg_length;
                i++;
            }
        }
        else {
            int countedPedestrians_left = countedPedestrians;
            int i = 0;
            while (countedPedestrians_left > 0) {
                countedPedestrians_segs[i] = 1;
                countedPedestrians_left--;
                i++;
            }
        }
    }

    std::stringstream jsonStr;

    jsonStr << "[";
    count = 0;
    pIt = detectedPeopleMap.begin();
    while (pIt != detectedPeopleMap.end()) {
        if (pIt->second.getDetected()) {
            if (count > 0) {
                jsonStr << " , ";
            }
            jsonStr << pIt->second.asJSON(frameNo, parameters->getunitGUID(), countedPedestrians_segs[count]);
            pIt->second.setSent(true);
            count++;
        }

        pIt++;
    }

    if (count == 0) {
        DetectedPerson *dummyPerson = createDummyProfile();
        jsonStr << dummyPerson->asJSON(frameNo, parameters->getunitGUID(), countedPedestrians);
    }

    jsonStr << "]";

    std::string str = jsonStr.str();
    mainLogger->info("json string to be sent prepared:");
    if (str.length() > 0) {
        if (parameters->geturl().compare("DEBUG") == 0) {
            mainLogger->debug(str.c_str());
        } else {
            RestClient::response r = RestClient::post(parameters->geturl(), "application/json", str, headers);
            if (r.code != 200) {
                mainLogger->error("POST failed, code %v", r.code);
                mainLogger->error("Failed to send");
            } else {
                mainLogger->info("Sent: %v", str.c_str());
            }
        }
    }

    std::cout << "[TEST] " << str << std::endl;
}

void Detector::InitOpenCV() {

    fileSource = false;
    if (!parameters->getinputFile().empty()) {
        mainLogger->info("Clip from file: %v", parameters->getinputFile());
        fileSource = true;
        videoCapture.open(parameters->getinputFile());
        if (!videoCapture.isOpened()) {
            mainLogger->error("Couldn't open %v", parameters->getinputFile());
        }

    } else if (!parameters->getinputStream().empty()) {
        videoCapture.open(parameters->getinputStream());
        if (!videoCapture.isOpened()) {
            mainLogger->error("Couldn't read streaming at %v", parameters->getinputStream());
        }

    } else {
        videoCapture.open(parameters->getinputCamera());
        if (!videoCapture.isOpened()) {
            mainLogger->error("Couldn't read the camera...!", parameters->getinputStream());
        }
        videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, parameters->getframe_width());
        videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, parameters->getframe_height());
        videoCapture.set(CV_CAP_PROP_FPS, parameters->getframe_rate());
    }

    window_name = "JC Video";

    if (!parameters->getis_display()) {
        cv::resizeWindow(window_name, 1280, 800);
        cvNamedWindow(window_name, cv::WINDOW_AUTOSIZE);
    }

    skip_frames = int(parameters->getframe_rate() / parameters->getdesired_frame_rate());

}

void Detector::InitPreProcessor() {

    preprocessor = new PreProcessor(*parameters);
}

void Detector::InitDetectors() {
    people_count_detector = new PeopleCountDetector(*parameters);
    face_detector = new FaceDetector(*parameters);
    if (parameters->getLicenseMode()) {
        face_detector->ShowSettings();
    }

    if (!face_detector->crowdSight) {
        status = false;
    }

}

void Detector::init() {

    status = true;

    if (parameters->getunitGUID() == -1) {
        mainLogger->error("unitGUID not set! unitGUID = %v", parameters->getunitGUID());
        status = false;
        return;
    }

    frameNo = 0;
    InitOpenCV();
    InitPreProcessor();
    InitDetectors();
    return;
}

bool Detector::ok() {
    return status;
}

void Detector::display(cv::Mat &frame, Parameters *parameters) {
    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 1;
    int thickness = 2;
    cv::Size s = frame.size();
    const vector<float> roi_fd = parameters->getroi();
    const vector<float> roi_vlines = parameters->getroi_vlines();
    if (roi_vlines.size() > 0) {
        Point pt1(int(roi_vlines[0] * s.width + 0.5), int(roi_vlines[1] * s.height + 0.5));
        Point pt2(int(roi_vlines[0] * s.width + 0.5), int(roi_vlines[3] * s.height + 0.5));
        Point pt3(int(roi_vlines[2] * s.width + 0.5), int(roi_vlines[1] * s.height + 0.5));
        Point pt4(int(roi_vlines[2] * s.width + 0.5), int(roi_vlines[3] * s.height + 0.5));
        cv::line(frame, pt1, pt2, vcolor::KCOLOR_GREEN_1, 4);
        cv::line(frame, pt3, pt4, vcolor::KCOLOR_GREEN_1, 4);
        cv::putText(frame, "green: line guards for people counting", pt1, fontFace, fontScale,
                    Scalar::all(255), thickness, 8);
    }
    if (roi_fd.size() > 0) {
        Rect roi = Rect(int(roi_fd[0] * s.width + 0.5), int(roi_fd[1] * s.height + 0.5),
                        int((roi_fd[2] - roi_fd[0]) * s.width + 0.5),
                        int((roi_fd[3] - roi_fd[1]) * s.height + 0.5));
        cv::rectangle(frame, roi, vcolor::KCOLOR_BLACK_1, 4);
        cv::putText(frame, "black: ROI for detecting face",
                    Point(int(roi_fd[0] * s.width + 0.5),
                          int(roi_fd[1] * s.height + 0.5)),
                    fontFace, fontScale,
                    Scalar::all(255), thickness, 8);
    }
    imshow(window_name, frame);
    static int count = 0;
    std::string filename = std::to_string(frameNo) + ".png";
    cv::imwrite(filename, frame);
    count++;
    waitKey(1);
}

void Detector::fd_roi_operators(cv::Mat& frame, cv::Mat& frame_roi) {
    const vector<float> roi_fd = parameters->getroi();
    float alpha_mask = 0.05f;
    unsigned char mask_value = 80;
    if (roi_fd.size() > 0) {
        // Rect(x, y, w, h);
        cv::Size s = frame.size();
        Rect roi = Rect(int(roi_fd[0] * s.width + 0.5), int(roi_fd[1] * s.height + 0.5),
                        int((roi_fd[2] - roi_fd[0]) * s.width + 0.5),
                        int((roi_fd[3] - roi_fd[1]) * s.height + 0.5));
        frame_roi = frame(roi);
    } else {
        frame.copyTo(frame_roi);
    }
    const vector<vector<float>> dontcare_rois = parameters->getdontcare_rois();
    if (dontcare_rois.size() > 0) {
        int width = frame_roi.cols;
        int height = frame_roi.rows;
        for (vector<float> a_roi : dontcare_rois) {
            int upper_left_x = int( a_roi[0] * width + 0.5 );
            int upper_left_y = int( a_roi[1] * height + 0.5 );
            int bottom_right_x = int( a_roi[2] * width + 0.5 );
            int bottom_right_y = int( a_roi[3] * height + 0.5 );

            for (int i = upper_left_x; i < bottom_right_x; i++) {
                for (int j = upper_left_y; j < bottom_right_y; j++) {
                    cv::Vec3b& pixels = frame_roi.at<Vec3b>(j, i);
                    pixels[0] = std::min(int(alpha_mask * pixels[0] + (1 - alpha_mask) * mask_value + 0.5), 255);
                    pixels[1] = std::min(int(alpha_mask * pixels[1] + (1 - alpha_mask) * mask_value + 0.5), 255);
                    pixels[2] = std::min(int(alpha_mask * pixels[2] + (1 - alpha_mask) * mask_value + 0.5), 255);
                }
            }

        }
    }
}

void Detector::run() {

    time_t analysisStartTime = time(0);

    cv::Mat frame;
    cv::Mat frame_roi;

    timeval start;
    gettimeofday(&start, NULL);

    mainLogger->debug("Started detections on unit: %v", parameters->getunitGUID());

    while(true) {
        videoCapture >> frame;
        frameNo++;
        if (frame.empty()) {
            videoCapture.release();
            break;
        }

        if ((frameNo % skip_frames) != 0) {
            continue;
        }
        preprocessed_frame = preprocessor->Process(frame);
        people_count_detector->line_det1->AddLine(preprocessed_frame);
        people_count_detector->line_det2->AddLine(preprocessed_frame);

        const vector<float> roi_fd = parameters->getroi();

        fd_roi_operators(frame, frame_roi);
        face_detector->Process(frame_roi, *parameters, frameNo, fileSource);

        if (parameters->getis_display()) {
            display(frame, parameters);
        }
    }
    people_count_detector->line_det1->DoDetection();
    people_count_detector->line_det2->DoDetection();

    countedPedestrians = int((people_count_detector->line_det1->detected_pedestrians + people_count_detector->line_det2->detected_pedestrians + 0.5) / 2.0);
    mainLogger->info("Number of pedestrians: %v", countedPedestrians);

    std::map<long, DetectedPerson>::iterator pIt;
    pIt = face_detector->detectedPersonMap.begin();
    while (pIt != face_detector->detectedPersonMap.end()) {
        DetectedPerson &person = pIt->second;
        if (!person.getDetected()) {
            pIt++;
            continue;
        }

        person.average_yaw_byCategory();
        person.setReportedPossibilityToSee();
        pIt++;
    }

    time_t endTime = time(0);
    mainLogger->debug("frame rate at frame [%v]: %v", frameNo, frameNo / (endTime - analysisStartTime));
}

