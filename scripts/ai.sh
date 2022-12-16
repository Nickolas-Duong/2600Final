#!/bin/bash

# Ask user what if pvp or pve
echo "Enter 'player' for player v player, or wait 5 seconds for AI to play"

# get choice
read -t 5 choice

#set count variable
count=0

#if player is chosen
if [[ $choice == "player" ]]; then
	# send player to main board
	sleep 2 && mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m "p"
	
	#while count is not 9
	while [ $count -le 8 ];
	do
		#Sleep and wait for esp32 input
		sleep 10
		#ask for player 2 input
		echo "Enter 1-9 to place an O | Enter 0 to quit the game"
		read num
		#if 0, then quit, otherwise play
		if [[ $num == "0" ]]; then
			count=$(($count + 10))
			echo "Quitting..."
			mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m $num
		else
			mqttx pub -t 'esp32/tictac' -h 'brokeer.emqx.io' -p 1883 -m $num
			count=$(($count + 1))
			echo $num
		fi
	done
	#echo $count
else
	#connect to ai game
	sleep 2 && mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m "a" &
	while [ $count -le 8 ];
	do
		#wait for esp32 input
		sleep 10
		#random generate a number, then publish
		num=$(($RANDOM % 9 + 1))
		mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m $num
		count=$(($count + 1))
		#echo $num
	done
	#echo $count
fi
