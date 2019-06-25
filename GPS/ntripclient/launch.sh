#!/bin/bash
SERVER="gnssdata.or.kr"
PORT=2101
MOUNT="SOUL-RTCM31"
USR="gnss"
PWD="gnss"
MODE="ntrip1"
SERDEV="/dev/ttyUSB0"
BAUD=9600

./ntripclient -s $SERVER -r $PORT -m $MOUNT -u $USR -p $PWD -M $MODE -D $SERDEV -B $BAUD

#./ntripclient -m SOUL-RTCM31 -s gnssdata.or.kr -p gnss -u gnss -M ntrip1 -D /dev/ttyUSB0
