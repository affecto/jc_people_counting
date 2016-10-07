import json
import sys, os
import subprocess
from subprocess import PIPE, Popen
from threading import Thread
from Queue import Queue, Empty
import datetime

ON_POSIX = 'posix' in sys.builtin_module_names

def output_from_binary(cmd):
    """from http://blog.kagesenshi.org/2008/02/teeing-python-subprocesspopen-output.html
    """
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout = []
    while True:
        line = p.stdout.readline()
        if '[TEST]' in line:
            stdout.append(line)
        print line,
        if line == '' and p.poll() != None:
            break
    return ''.join(stdout)

root = '/home/amin/ClionProjects/jc_people_counting/'
with open(root + 'configurations/conf_local.json', 'r') as json_data_file:
    data = json.load(json_data_file)

'''parameters for cam2-20160921080253.mp4
  "roi" : [0.05, 0.15, 0.9, 0.85],
  "roi_vlines" : [0.1, 0.2, 0.7, 1.0],
  "dontcare_rois" : [[0.58, 0.0, 0.75, 0.15], [0.65, 0.15, 0.75, 0.35], [0.74, 0.4, 0.78, 0.5]]'''

# roi = [0.0, 0.0, 1.0, 1.0]
# dontcare_rois = []

# combinations
cs_use_fastdetection = [True, False]
cs_FaceDetector = [0, 1, 2, 5]
cs_acceptance_threshold = [0.1, 0.2, 0.3, 0.4, 0.5]

idx = 0
script_list = []
for use_fastdetection in cs_use_fastdetection:
    data['cs_use_fastdetection'] = use_fastdetection
    for facedetector in cs_FaceDetector:
        data['cs_FaceDetector'] = facedetector
        for accept in cs_acceptance_threshold:
            data['cs_acceptance_threshold'] = accept
            filename = root + 'configurations/%d.json' % (idx)
            with open(filename, 'w') as outfile:
                json.dump(data, outfile, indent=4)
                script_list.append(filename)
                idx += 1

program_binary = root + 'build/jc_pilot'
log_filename = 'output.log'
with open(log_filename, 'w') as log_file:
    for script in script_list:
        with open(script, 'r') as json_data_file:
            data = json.load(json_data_file)

        input_arguments = program_binary + ' ' + script + ' 1'
        print 'cmd = ', input_arguments

        t1 = datetime.datetime.now()
        line = output_from_binary(input_arguments)
        if line == '':
            line = 'NULL'

        t2 = datetime.datetime.now()
        head, tail = os.path.split(script)
        log_file.write(tail + ' ' + '(%s, %s, %s) ' % \
                       (str(data["cs_use_fastdetection"]), str(data["cs_FaceDetector"]), str(data["cs_acceptance_threshold"])) \
                       + str((t2 - t1).seconds) + 'sec. ' + str(line))