import json
import sys, os
import subprocess
import itertools
from subprocess import PIPE, Popen
from threading import Thread
from Queue import Queue, Empty
import datetime

ON_POSIX = 'posix' in sys.builtin_module_names
root = '/home/amin/ClionProjects/jc_people_counting/'
program_binary = root + 'build/jc_pilot'
log_filename = 'output_varying_scale07_minface40.log'

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

with open(root + 'configurations/conf_local.json', 'r') as json_data_file:
    data = json.load(json_data_file)

'''parameters for cam2-20160921080253.mp4
  "roi" : [0.05, 0.15, 0.9, 0.85],
  "roi_vlines" : [0.1, 0.2, 0.7, 1.0],
  "dontcare_rois" : [[0.58, 0.0, 0.75, 0.15], [0.65, 0.15, 0.75, 0.35], [0.74, 0.4, 0.78, 0.5]]'''

# roi = [0.0, 0.0, 1.0, 1.0]
# dontcare_rois = []

# test combinations
cs_FaceDetector = [1, 5]
cs_acceptance_threshold = [0.4, 0.5]

inputFile = ['/home/amin/ClionProjects/jc_people_counting/videos/cam1-20161004000015.mp4',
             '/home/amin/ClionProjects/jc_people_counting/videos/Eliel_1.mp4',
             '/home/amin/ClionProjects/jc_people_counting/videos/cam2-20160921080253.mp4',
             '/home/amin/ClionProjects/jc_people_counting/videos/cam2-20160921080353.mp4']

idx = 0
script_list = []
for (input, accept, facedetector) in itertools.product(*[inputFile, cs_acceptance_threshold, cs_FaceDetector]):
    filename = root + 'configurations/%d.json' % (idx)
    data['inputFile'] = input
    data['cs_acceptance_threshold'] = accept
    data['cs_FaceDetector'] = facedetector
    if facedetector == 1:
        skip_frame = 2
    else:
        skip_frame = 4
    data['fd_skip_frames'] = skip_frame
    filename = root + 'configurations/%d.json' % (idx)
    with open(filename, 'w') as outfile:
        json.dump(data, outfile, indent=4)
        script_list.append(filename)
        idx += 1

with open(log_filename, 'w') as log_file:
    for script in script_list:
        with open(script, 'r') as json_data_file:
            data = json.load(json_data_file)

        input_arguments = program_binary + ' ' + script
        print 'cmd = ', input_arguments

        t1 = datetime.datetime.now()
        line = output_from_binary(input_arguments)
        if line == '':
            line = 'NULL\n'

        t2 = datetime.datetime.now()
        head, tail = os.path.split(script)
        head, mp4_filename = os.path.split(data['inputFile'])
        log_file.write(tail + ' ' + '(%s, %s, %s, %s) ' % \
                       (data["inputFile"], str(data["cs_acceptance_threshold"]), str(data["cs_FaceDetector"]), str(data["fd_skip_frames"])) \
                       + str((t2 - t1).seconds) + 'sec. ' + str(line))