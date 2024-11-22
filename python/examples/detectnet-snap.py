

import argparse
import datetime
import math
import sys
import os

from jetson_inference import detectNet
from jetson_utils import (videoSource, videoOutput, saveImage, Log,
                          cudaAllocMapped, cudaCrop, cudaDeviceSynchronize)

# parse the command line
parser = argparse.ArgumentParser(description="Locate objects in a live camera stream using an object detection DNN.", 
                                 formatter_class=argparse.RawTextHelpFormatter, 
                                 epilog=detectNet.Usage() + videoSource.Usage() + videoOutput.Usage() + Log.Usage())

parser.add_argument("input", type=str, default="", nargs='?', help="URI of the input stream")
parser.add_argument("output", type=str, default="", nargs='?', help="URI of the output stream")
parser.add_argument("--network", type=str, default="ssd-mobilenet-v2", help="pre-trained model to load (see below for options)")
parser.add_argument("--overlay", type=str, default="box,labels,conf", help="detection overlay flags (e.g. --overlay=box,labels,conf)\nvalid combinations are:  'box', 'labels', 'conf', 'none'")
parser.add_argument("--threshold", type=float, default=0.5, help="minimum detection threshold to use") 
parser.add_argument("--snapshots", type=str, default="images/test/detections", help="output directory of detection snapshots")
parser.add_argument("--timestamp", type=str, default="%Y%m%d-%H%M%S-%f", help="timestamp format used in snapshot filenames")

try:
    args = parser.parse_known_args()[0]
except:
    print("")
    parser.print_help()
    sys.exit(0)

# make sure the snapshots dir exists
os.makedirs(args.snapshots, exist_ok=True)

# create video output object 
output = videoOutput(args.output, argv=sys.argv)
	
# load the object detection network
net = detectNet(args.network, sys.argv, args.threshold)

# create video sources
input = videoSource(args.input, argv=sys.argv)

# process frames until EOS or the user exits
while True:
    # capture the next image
    img = input.Capture()

    if img is None: # timeout
        continue  

    # detect objects in the image (with overlay)
    detections = net.Detect(img, overlay=args.overlay)

    # print the detections
    print("detected {:d} objects in image".format(len(detections)))

    timestamp = datetime.datetime.now().strftime(args.timestamp)

    for idx, detection in enumerate(detections):
        print(detection)
        roi = (int(detection.Left), int(detection.Top), int(detection.Right), int(detection.Bottom))
        snapshot = cudaAllocMapped(width=roi[2]-roi[0], height=roi[3]-roi[1], format=img.format)
        cudaCrop(img, snapshot, roi)
        cudaDeviceSynchronize()
        saveImage(os.path.join(args.snapshots, f"{timestamp}-{idx}.jpg"), snapshot)
        del snapshot

    # render the image
    output.Render(img)

    # update the title bar
    output.SetStatus("{:s} | Network {:.0f} FPS".format(args.network, net.GetNetworkFPS()))

    # print out performance info
    net.PrintProfilerTimes()

    # exit on input/output EOS
    if not input.IsStreaming() or not output.IsStreaming():
        break
        