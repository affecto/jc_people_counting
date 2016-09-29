#!/usr/bin/env bash

STREAM="rtsp://affadmin:aff1234@192.168.1.150/main"
CAM="cam2"
CAPTURE_DIR="/home/ubuntu/data/"
FORMAT="-%04Y%02m%02d%02H%02M%02S.mp4"

ffmpeg -report -v 8 -i $STREAM -map 0:0 -c:v copy -f segment -segment_format mp4 -segment_time 60 -segment_list $CAPTURE_DIR$CAM.csv -strftime 1 $CAPTURE_DIR$CAM$FORMAT &
pid=$!
echo "ffmpeg pid: $pid"

while true; do

if ps -p $pid > /dev/null; then
  echo "capturing on: do nothing"
  echo "pid: $pid"
else
  ffmpeg -report -v 8 -i $STREAM -map 0:0 -c:v copy -f segment -segment_format mp4 -segment_time 60 -segment_list $CAPTURE_DIR$CAM.csv -strftime 1 $CAPTURE_DIR$CAM$FORMAT &
  pid=$!
  echo "restarting capturing: pid: $pid"
fi
sleep 10
done

