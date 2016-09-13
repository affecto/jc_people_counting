On the NUC, start these in screen:

```
python /home/ubuntu/lc-file-transfer/capture_video.py --ffmpeg /opt/ffmpeg-3.0.2-64bit-static/ffmpeg --stream rtsp://admin:ms1234@192.168.1.150/main --camera cam1 --capturedir /home/ubuntu/data/

python /home/ubuntu/lc-file-transfer/send_files.py --hostname 52.58.86.18 --username ubuntu --capturedir /home/ubuntu/data/ --capturecsv cam1.csv --transferdir /data/video/cam1/ --transfercsv sent.csv
```

On the cloud:

```
python ./receive_files.py --detector /home/ubuntu/ultra_simple_detector --license 158d9b68ef3343ffb3d1d99dd69d173a --url https://7lv76wlvqk.execute-api.eu-west-1.amazonaws.com/prod/upload --id 12345 --transferdir /data/video/cam1/ --transfercsv sent.csv
```

