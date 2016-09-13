#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from glob import glob
from subprocess import Popen
from time import sleep, time
import datetime
import argparse
import shutil


def is_mp4(filename):
    try:
        f = open(filename, 'rb')
        d = f.read(8)
        f.close()
        return d[4] == 'f' and d[5] == 't' and d[6] == 'y' and d[7] == 'p'
    except:
        return False


parser = argparse.ArgumentParser(description='Make sure ffmpeg capture is running')

parser.add_argument('--ffmpeg', type=str,
                    help='the path to the ffmpeg binary you want to use',
                    default='ffmpeg')

parser.add_argument('--stream', type=str,
                    help='the RTSP stream URL',
                    default='rtsp://admin:ms1234@192.168.1.150/main')

parser.add_argument('--camera_id', type=str,
                    help='laptop camera id',
                    default='')
parser.add_argument('--camera', type=str, help='the camera name', default='cam1')

parser.add_argument('--capturedir', type=str, required=True,
                    help='the directory into which MP4 files are stored',
                    default=os.getcwd())

parser.add_argument('--encrypteddir', type=str, required=True,
                    help='the directory into which encrypted files are saved')

parser.add_argument('--sleep_start_time_hour', type=int, required=True, help="The beginning of non-capturing time in hours", default=2)
parser.add_argument('--sleep_start_time_min', type=int, required=True, help="The beginning of non-capturing time in hours", default=0)
parser.add_argument('--sleep_end_time_hour', type=int, required=True, help="The beginning of non-capturing time in hours", default=6)
parser.add_argument('--sleep_end_time_min', type=int, required=True, help="The beginning of non-capturing time in hours", default=0)

args = parser.parse_args()

if args.camera_id != '':
    ARGS = [
        args.ffmpeg,
        '-v', '8',
        '-i', '/dev/video0',
        '-map', '0:0',
        '-c:v', 'copy',
        '-f', 'segment',
        '-segment_format', 'mp4',
        '-segment_time', '60',
        '-segment_list', args.capturedir + args.camera + '.csv',
        '-strftime', '1',
                         args.capturedir + args.camera + '-%04Y%02m%02d%02H%02M%02S.mp4'
    ]
else:
    ARGS = [
        args.ffmpeg,
        '-v', '8',
        '-i', args.stream,
        '-map', '0:0',
        '-c:v', 'copy',
        '-f', 'segment',
        '-segment_format', 'mp4',
        '-segment_time', '60',
        '-segment_list', args.capturedir + args.camera + '.csv',
        '-strftime', '1',
        args.capturedir + args.camera + '-%04Y%02m%02d%02H%02M%02S.mp4'
]

p = None

try:
    while True:
        print 'Starting ffmpeg...'

        # If time is between sleep_start_time and sleep_send_time stop process if on
        now = datetime.datetime.now()

        if (now.hour >= args.sleep_start_time_hour and now.hour <= args.sleep_end_time_hour):
            if (now.minute >= args.sleep_start_time_min and now.minute <= args.sleep_end_time_min):
                if p:
                    p.kill()
                sleep(10)
                continue

        p = Popen(ARGS)
        sleep(3)

        print 'Polling for problems...'
        while True:
            sleep(1)

            p.poll()
            if p.returncode:
                break

            latest_mtime = 0
            pass_time = time()

            for fn in glob(args.capturedir + args.camera + '-*.mp4'):
                try:
                    (mode, ino, dev, nlink, uid, gid, size, atime, mtime, ctime) = os.stat(fn)

                    if latest_mtime < mtime:
                        latest_mtime = mtime

                    if pass_time - mtime > 120 and is_mp4(fn):
                        print 'Encrypting', fn
                        ep = Popen(['gpg', '--batch', '--quiet',
                                    '-e', '-r', '31758A2B', '-o', fn + '.encrypted', fn])
                        ep.wait()
                        shutil.move(fn + '.encrypted', fn)

                except OSError:
                    # File was deleted by the transfer script while we were globbing
                    pass

            if latest_mtime == 0 or pass_time - latest_mtime > 120:
                print 'looks like ffmpeg has stopped producing, killing it...'
                try:
                    p.kill()
                except OSError:
                    pass
                break

            now = datetime.datetime.now()

            if (now.hour >= args.sleep_start_time_hour and now.hour <= args.sleep_end_time_hour):
                if (now.minute >= args.sleep_start_time_min and now.minute <= args.sleep_end_time_min):
                    if p:
                        p.kill()
                    break

        print 'ffmpeg has stopped'
        sleep(10)

except KeyboardInterrupt:
    print "shutting down on request..."
    if p:
        p.kill()
    exit(1)
