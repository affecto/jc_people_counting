#!/usr/bin/env bash

isRunning()
{
date=$('date')
case "$(pgrep -f "capture_stream.sh" | wc -w)" in

0) echo "$date" "video stream capture not running, restarting capture"
/bin/bash /home/ubuntu/capture_stream.sh &
;;
1) echo "$date" "video stream capture running"
;;
esac

date=$('date')
case "$(pgrep -f "compress_and_encrypt_files.sh" | wc -w)" in

0) echo "$date" "video processing not running, restarting processing"
/bin/bash /home/ubuntu/compress_and_encrypt_files.sh &
;;
1) echo "$date" "video processing running"
;;
esac

date=$('date')
case "$(pgrep -f "send_encrypted_files.sh" | wc -w)" in

0) echo "$date" "video sending not running, restarting sending"
/bin/bash /home/ubuntu/send_encrypted_files.sh &
;;
1) echo "$date" "video sending running"
;;
esac
}

while true; do
isRunning
sleep 60
done