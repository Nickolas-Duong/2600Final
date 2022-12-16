#!/bin/sh

#create terminals for ai and main board
gnome-terminal --working-directory /home/krievoblast/Desktop/hwtwo/2600Final/scripts -x bash -c "./ai.sh; exec bash" &
gnome-terminal --working-directory /home/krievoblast/Desktop/hwtwo/2600Final/bin -x bash -c "./main ; exec bash" &

