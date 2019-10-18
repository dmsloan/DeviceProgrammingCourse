#!/bin/bash
if [ $# -eq 0 ]
  then
        echo Deploying to Pi
        ADDRESS="192.168.0.15"
        USERNAME="pi"
        PASSWORD="raspberry"
        DIRECTORY="IntroToDeviceProgramming"

        sshpass -p $PASSWORD ssh $USERNAME@$ADDRESS "rm -r $DIRECTORY; mkdir $DIRECTORY"
        sshpass -p $PASSWORD scp ./main.c ./CMakeLists.txt "$USERNAME@$ADDRESS:/home/pi/$DIRECTORY"

        sshpass -p $PASSWORD ssh $USERNAME@$ADDRESS "cd ./$DIRECTORY; cmake . && make; sudo ./app"
    else
        echo Deploying Locally
        cmake ./ && make && sudo ./app
fi