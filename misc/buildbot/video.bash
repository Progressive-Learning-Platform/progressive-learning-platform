#!/bin/bash

$INFILE=$1
$LECTURE=$2

#encode the video
ffmpeg -i $1 /backup/fritz/ecen4243spring2011_videos/lecture${2}.webm

#send it off!
scp /backup/fritz/ecen4243spring2011_videos/lecture${2}.webm fritz@rome.ceat.okstate.edu:~/plp/ecen4243/videos

#we're done, let me add the link
echo "Done. Don't forget to link to the video!"
