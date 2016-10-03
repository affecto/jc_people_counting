//
// Created by kuoppves on 20.8.2016.
//

#include "Parameters.h"
using namespace rapidjson;
extern el::Logger* mainLogger;

Parameters::Parameters(const char* filename, std::string deviceID) {
    char buffer[65536];
    FILE *fp = fopen(filename, "rb");

    if (fp != NULL)
        FileReadStream(fp, buffer, sizeof(buffer));
    else {
        mainLogger->error("cannot open %v", filename);
        fclose(fp);
        return;
    }

    fclose(fp);

    mainLogger->info("Access values in document %v", filename);
    if (configuration.ParseInsitu(buffer).HasParseError()) {
        mainLogger->error("failed to parse configuration file %v (file exists though)", filename);
        return;
    }

    assert(configuration.IsObject());

    mainLogger->info("Finished accessing values in document %v", filename);
    if (configuration.HasMember("unitGUID")) unitGUID = configuration["unitGUID"].GetInt();
    if (configuration.HasMember("deviceId")) deviceId = configuration["deviceId"].GetString();
    if (configuration.HasMember("conf_file")) conf_file = configuration["conf_file"].GetString();
    if (configuration.HasMember("is_dev")) is_dev = configuration["is_dev"].GetBool();
    if (configuration.HasMember("conf_file")) conf_file = configuration["conf_file"].GetString();
    if (configuration.HasMember("crowdsight_license")) crowdsight_license = configuration["crowdsight_license"].GetString();
    if (configuration.HasMember("crowdsight_datadir")) crowdsight_datadir = configuration["crowdsight_datadir"].GetString();

    if (configuration.HasMember("inputCamera")) inputCamera = configuration["inputCamera"].GetInt();
    if (configuration.HasMember("frame_width")) frame_width = configuration["frame_width"].GetInt();
    if (configuration.HasMember("frame_height")) frame_height = configuration["frame_height"].GetInt();

    if (configuration.HasMember("frame_rate")) frame_rate = configuration["frame_rate"].GetDouble();
    if (configuration.HasMember("desired_frame_rate")) desired_frame_rate = configuration["desired_frame_rate"].GetDouble();

    if (configuration.HasMember("inputFile")) inputFile = configuration["inputFile"].GetString();
    if (configuration.HasMember("inputStream")) inputStream = configuration["inputStream"].GetString();
    if (configuration.HasMember("url")) url = configuration["url"].GetString();

    if (configuration.HasMember("is_display")) is_display = configuration["is_display"].GetBool();

    if (configuration.HasMember("scale_factor")) scale_factor = configuration["scale_factor"].GetDouble();
    if (configuration.HasMember("use_grayscale")) use_grayscale = configuration["use_grayscale"].GetBool();

    if (configuration.HasMember("roi")) {
        const Value& roi_fd = configuration["roi"];
        assert(roi_fd.IsArray() && roi_fd.Size() == 4);
        roi.clear();
        for (SizeType i = 0; i < roi_fd.Size(); i++)
            roi.push_back(float(roi_fd[i].GetDouble()));
    }
    if (configuration.HasMember("roi_vlines")) {
        const Value& roi_vl = configuration["roi_vlines"];
        assert(roi_vl.IsArray() && roi_vl.Size() == 4);
        roi_vlines.clear();
        for (SizeType i = 0; i < roi_vl.Size(); i++)
            roi_vlines.push_back(float(roi_vl[i].GetDouble()));
    }

    if (configuration.HasMember("startSecond")) startSecond = configuration["startSecond"].GetInt();
    if (configuration.HasMember("post_interval")) post_interval = configuration["post_interval"].GetDouble();
    if (configuration.HasMember("ad_point_x")) ad_point_x = configuration["ad_point_x"].GetInt();
    if (configuration.HasMember("ad_point_y")) ad_point_y = configuration["ad_point_y"].GetInt();

    if (configuration.HasMember("detectionCount")) detectionCount = configuration["detectionCount"].GetInt();
    if (configuration.HasMember("observationCountInterval")) observationCountInterval = configuration["observationCountInterval"].GetInt();
    if (configuration.HasMember("observationDeprecationTime")) observationDeprecationTime = configuration["observationDeprecationTime"].GetInt();
    if (configuration.HasMember("reactFramesThreshold")) reactFramesThreshold = configuration["reactFramesThreshold"].GetInt();
    if (configuration.HasMember("reactObservationDeprecationTime")) reactObservationDeprecationTime = configuration["reactObservationDeprecationTime"].GetInt();
    if (configuration.HasMember("blob_area_threshold")) blob_area_threshold = configuration["blob_area_threshold"].GetInt();
    if (configuration.HasMember("scheduled_sleep")) scheduled_sleep = configuration["scheduled_sleep"].GetInt();

    if (configuration.HasMember("schedu1ed_sleep_start")) schedu1ed_sleep_start = configuration["schedu1ed_sleep_start"].GetString();
    if (configuration.HasMember("scheduled_sleep_stop")) scheduled_sleep_stop = configuration["scheduled_sleep_stop"].GetString();

    if (configuration.HasMember("min_face_size")) min_face_size = configuration["min_face_size"].GetInt();
    if (configuration.HasMember("is_debug_image")) is_debug_image = configuration["is_debug_image"].GetBool();
    if (configuration.HasMember("detectionCount")) detectionCount = configuration["detectionCount"].GetInt();
}

Parameters::~Parameters() {


}
