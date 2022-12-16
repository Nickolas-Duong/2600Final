#!/bin/sh

gnome-terminal --working-directory /home/krievoblast/Desktop/hwtwo/2600Final -x bash -c "./ai.sh; exec bash" &
gnome-terminal --working-directory /home/krievoblast/Desktop/hwtwo/2600Final -x bash -c "./test ; exec bash" &

