#!/usr/bin/env bash
CAM="cam2"
DATA_DIR="/home/ubuntu/data/"
pushd $DATA_DIR

password_file="/home/ubuntu/password"

while true; do

i=0
for i in "$CAM"*.mp4; do
    video_file=$i
    echo [$i] "$video_file"
    who_has_it=$(lsof -w "$video_file")
    if [[ -z $who_has_it ]] ; then
    
        ffmpeg -y -i "$video_file" -c:v libx264 -preset medium -b:v 788k -pass 1 -f mp4 /dev/null &&
        ffmpeg -i "$video_file" -c:v libx264 -preset medium -b:v 788k -pass 2 compressed-"${video_file}"
        openssl aes-128-cbc -e -in compressed-"$video_file" -out encrypted_"$video_file" -salt  -pass file:"$password_file"        
        rm "$video_file"
        rm compressed-"$video_file"
    else
        echo "file in use"
    fi
done
sleep 30
done
popd


