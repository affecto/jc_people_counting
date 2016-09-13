//
// Created by kuoppves on 21.8.2016.
//

#include "DetectedPerson.h"

using namespace rapidjson;

std::string DetectedPerson::asJSON(int frameID, int unitID) {

    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();

    writer.String("frameID");
    writer.Int64(frameID);

    writer.String("unitGUID");
    writer.Int64(unitID);

    writer.String("ID");
    writer.Int64(crowdSightID);

    writer.String("firstObserved");
    writer.Int64(firstObservedTimestamp);

    writer.String("lastObserved");
    writer.Int64(lastObserved);

    writer.String("age");
    writer.Double(getAge());

    writer.String("gender");
    writer.Double(getGender());

    writer.String("gazex");
    writer.Int64(gaze.x);

    writer.String("gazey");
    writer.Int64(gaze.y);

    writer.String("attentionSpan");
    writer.Int64(attentionSpan);

    writer.EndObject();

    return s.GetString();
}