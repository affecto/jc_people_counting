#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, shutil
import datetime
import argparse
import logging
import json
from time import sleep
import subprocess
from subprocess import Popen
from glob import glob

from arrow import Arrow
from dateutil import tz

#json_filename = 'configurations/conf_lasi.json'

EPOCH = datetime.datetime(1970, 1, 1, tzinfo=tz.gettz('UTC'))
HELSINKI = tz.gettz('Europe/Helsinki')

parser = argparse.ArgumentParser(description='Receive MP4 files from remote controllers')

parser.add_argument('--transferdir', type=str, help='the directory oon the remote SSH server to send the files',
                    default='/home/ubuntu/video/cam1/')

parser.add_argument('--backupdir', type=str,
                    help='if you want to move the processed MP4 files instead of deleting them')

parser.add_argument('--detector', required=True)
parser.add_argument('--cfg', required=True)
parser.add_argument('--password', type=str, default='/home/kuoppves/Projects/jc_decoding_test/password')
args = parser.parse_args()


logging.basicConfig(filename='receive_encoded_files_jc10.log', level=logging.DEBUG, format='%(asctime)s %(message)s')


def is_reserved(filepath):
    try:
        result = subprocess.check_output(['fuser', fn])
    #process = Popen(['fuser', filepath], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #lines = process.stdout.readlines()
    #process.wait()
    except subprocess.CalledProcessError:
        result = ''

    if len(result) == 0:
        return False
    else:
        return True

def is_locked(filepath):
    """Checks if a file is locked by opening it in append mode.
    If no exception thrown, then the file is not locked.
    """
    locked = None
    file_object = None
    if os.path.exists(filepath):
        try:
            print "Trying to open %s." % filepath
            buffer_size = 8
            # Opening file in append mode and read the first 8 characters.
            file_object = open(filepath, 'a', buffer_size)
            if file_object:
                print "%s is not locked." % filepath
                locked = False
        except IOError, message:
            print "File is locked (unable to open in append mode). %s." % \
                  message
            locked = True
        finally:
            if file_object:
                file_object.close()
                print "%s closed." % filepath
    else:
        print "%s not found." % filepath
    return locked

p = None

try:
    while True:
        print 'Checking for files....'

        try:
            for fn in glob(args.transferdir + 'encrypted_*.mp4'):
                try:
                    if is_reserved(fn):
                        continue
                    else:
                        if os.access(fn, os.R_OK):
                            print 'Processing', fn

                            try:
                                print 'Decrypting', fn
                                out = fn + ".decrypted"
                                password = 'file:' + args.password
                                ep = Popen(['openssl', 'aes-128-cbc', '-d', '-in', fn, '-out', out, '-salt', '-pass', password])
                                ep.wait()
                                # Strip filename
                                basename = os.path.basename(fn)
                                base = basename.split('_')[1]
                                outfile = args.transferdir + base
                                shutil.move(out, outfile)

                                stamp = Arrow.strptime(outfile.split('-')[1][:-4], '%Y%m%d%H%M%S', HELSINKI)
                                t1 = datetime.datetime.now()

                                with open(json_filename, 'r') as json_data_file:
                                    data = json.load(json_data_file)

                                data['inputFile'] = outfile
                                data['startSecond'] = stamp.timestamp

                                with open(json_filename, 'w') as json_data_file:
                                    json.dump(data, json_data_file, indent=4)

                                p = Popen([args.detector, '--cfg', args.cfg])
                                p.wait()
                                t2 = datetime.datetime.now()

                                if args.backupdir:
                                    shutil.move(outfile, args.backupdir)
                                else:
                                    os.remove(outfile)
                                    os.remove(fn)
                                print 'Done with', outfile, "in", (t2 - t1).seconds, "seconds"
                            except IOError:
                                pass
                except IOError:
                    pass

        except IOError:
            pass

        sleep(1)

except KeyboardInterrupt:
    print "shuttng down on request..."
    if p:
        p.kill()

    exit(1)
