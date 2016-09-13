#!/usr/bin/env bash


RECORDER='ffmpeg'
STREAM=''
CAMERA_DEVICE='/dev/video0'
CAMERA_ID='test_video'
CAPTURE_DIR='/home/kuoppves/capture/'
ENCRYPT_DIR='/home/kuoppves/encrypt/'
SLEEP_START_TIME_HOUR=2
SLEEP_START_TIME_MIN=0
SLEEP_END_TIME_HOUR=6
SLEEP_END_TIME_MIN=0

python capture_video.py --ffmpeg $RECORDER --stream STREAM --camera_id $CAMERA_DEVICE --camera $CAMERA_ID --capturedir $CAPTURE_DIR --encrypteddir $ENCRYPT_DIR --sleep_start_time_hour $SLEEP_START_TIME_HOUR --sleep_start_time_min $SLEEP_START_TIME_MIN --sleep_end_time_hour $SLEEP_END_TIME_HOUR --sleep_end_time_min $SLEEP_END_TIME_MIN
