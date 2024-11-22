

import sys
import argparse

from jetson_inference import actionNet
from jetson_utils import videoSource, videoOutput, cudaFont, Log

# parse the command line
parser = argparse.ArgumentParser(description="Classify the action/activity of an image sequence.", 
                                 formatter_class=argparse.RawTextHelpFormatter, 
                                 epilog=actionNet.Usage() + videoSource.Usage() + videoOutput.Usage() + Log.Usage())

parser.add_argument("input", type=str, default="", nargs='?', help="URI of the input stream")
parser.add_argument("output", type=str, default="", nargs='?', help="URI of the output stream")
parser.add_argument("--network", type=str, default="resnet-18", help="pre-trained model to load (see below for options)")

try:
    args = parser.parse_known_args()[0]
except:
    print("")
    parser.print_help()
    sys.exit(0)


# load the recognition network
net = actionNet(args.network, sys.argv)

# create video sources & outputs
input = videoSource(args.input, argv=sys.argv)
output = videoOutput(args.output, argv=sys.argv)
font = cudaFont()

# process frames until EOS or the user exits
while True:
    # capture the next image
    img = input.Capture()

    if img is None: # timeout
        continue  

    # classify the action sequence
    class_id, confidence = net.Classify(img)
    class_desc = net.GetClassDesc(class_id)
    
    print(f"actionnet:  {confidence * 100:2.5f}% class #{class_id} ({class_desc})")
    
    # overlay the result on the image	
    font.OverlayText(img, img.width, img.height, "{:05.2f}% {:s}".format(confidence * 100, class_desc), 5, 5, font.White, font.Gray40)

    # render the image
    output.Render(img)

    # update the title bar
    output.SetStatus("actionNet {:s} | Network {:.0f} FPS".format(net.GetNetworkName(), net.GetNetworkFPS()))

    # print out performance info
    net.PrintProfilerTimes()

    # exit on input/output EOS
    if not input.IsStreaming() or not output.IsStreaming():
        break
