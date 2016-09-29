#!/usr/bin/env bash
CAM="cam2"
USERNAME="ubuntu"
HOSTNAME="52.58.86.18"
DATA_DIR="/home/ubuntu/data/"
pushd $DATA_DIR

password_file="/home/ubuntu/password"

while true; do

i=0
for i in encrypted*.mp4; do
    video_file=$i
    echo [$i] "$video_file"
    who_has_it=$(lsof -w "$video_file")
    if [[ -z $who_has_it ]] ; then
        scp $video_file $ubuntu@$HOSTNAME:/data/video/cam2/
        rm "$video_file"
    else
        echo "file in use"
    fi
done
done
popd
