#!/bin/bash

echo "Enter 'player' for player v player"

read -t 5 choice

count=0

if [[ $choice == "player" ]]; then
	sleep 2 && mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m "p"
	while [ $count -le 8 ];
	do
		#sleep 10
		echo "Enter 1-9 to place an O | Enter 0 to quit the game"
		read num
		if [[ $num == "0" ]]; then
			count=$(($count + 10))
			echo "Quitting..."
		else
			mqttx pub -t 'esp32/tictac' -h 'brokeer.emqx.io' -p 1883 -m $num
			count=$(($count + 1))
			echo $num
		fi
	done
	echo $count
else
	sleep 2 && mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m "a" &
	while [ $count -le 8 ];
	do
		sleep 10
		num=$(($RANDOM % 9 + 1))
		mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m $num
		count=$(($count + 1))
		echo $num
	done
	echo $count
fi
