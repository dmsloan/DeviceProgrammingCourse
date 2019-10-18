#ifndef WIRING_H_
#define WIRING_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __arm__
#include <wiringPi.h>
#include <wiringPiSPI.h>
#else
#include <unistd.h>

#define HIGH 1
#define LOW 0
#define OUTPUT 0
#define INPUT 1
#endif

int setupWiringPi()
{
    #ifdef __arm__
        return wiringPiSetup();
    #else
        return 0; //no-op if not on Pi
    #endif
}

void setPinMode(int pinNumber, int mode)
{
    #ifdef __arm__
        pinMode(pinNumber, mode);
    #else
        return;   //no-op if not on Pi
    #endif
}

void writeToPin(int pinNumber, int value)
{
    #ifdef __arm__
        digitalWrite(pinNumber, value);
    #else
        return; //no-op if not on Pi
    #endif
}

void delayOnPC(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void wait(int duration)
{
    fflush(stdout);

    #ifdef __arm__
        delay(duration);
    #else
        delayOnPC(duration * 1000);
    #endif
}

#endif // WIRING_H_
