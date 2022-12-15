#!/bin/bash

./test &
(sleep 10 && mqttx pub -t 'esp32/tictac' -h 'broker.emqx.io' -p 1883 -m "1") &

