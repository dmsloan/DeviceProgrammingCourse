#include <stdio.h>
#include <stdbool.h>
#include "./wiring.h"

int main(int argc, char *argv[])
{
    // ensure that wiringPiSetup is completed when running on a Pi
    setupWiringPi();

    // configure the pin numbers for the three LEDs
    int bitValue1Pin = 0;   //Physical pin 11 (GPIO 17)
    int bitValue2Pin = 1;   //Physical pin 12 (GPIO 18)
    int bitValue4Pin = 2;   //Physical pin 13 (GPIO 27)

    // configure the pinMode to OUTPUT
    setPinMode(bitValue1Pin, OUTPUT);
    setPinMode(bitValue2Pin, OUTPUT);
    setPinMode(bitValue4Pin, OUTPUT);

    // configure variables to hold the ON/OFF state of each of the LEDs
    bool led1OnOffState; // true = 1 = ON (HIGH), false = 0 = OFF (LOW)
    bool led2OnOffState;
    bool led3OnOffState;

    // the number of times to cycle through the values 0 through 7
    int iterations = 2;

    for (int iteration = 0; iteration < iterations; iteration++)
    {
            // a for loop that counts from 0 to 7
        for (int i = 0; i < 8; i++)
        {
            // check to see if the 1, 2, and 4 bits are set in the current number
            led1OnOffState = (i & 1); // check for binary bit value 1 = true
            led2OnOffState = (i & 2); // check for binary bit value 2 = true
            led3OnOffState = (i & 4); // check for binary bit value 4 = true

            printf("The decimal %d is %d%d%d in binary\n", i, led3OnOffState, led2OnOffState, led1OnOffState);

            // control the LEDs
            writeToPin(bitValue1Pin, led1OnOffState);
            writeToPin(bitValue2Pin, led2OnOffState);
            writeToPin(bitValue4Pin, led3OnOffState);

            /* control the LEDs
            if (led1OnOffState == true){
                writeToPin(bitValue1Pin, HIGH);
            } else {
                writeToPin(bitValue1Pin, LOW);
            }

            if (led2OnOffState == true){
                writeToPin(bitValue2Pin, HIGH);
            } else {
                writeToPin(bitValue2Pin, LOW);
            }

            if (led1OnOffState == true){
                writeToPin(bitValue4Pin, HIGH);
            } else {
                writeToPin(bitValue4Pin, LOW);
            } */

            // keep the current binary count displayed for a half second
            delay(500);
        }
    }

    return 0;
}

/*
#include <stdio.h>
#include "./wiring.h"

int main(int argc, char *argv[])
{
    setupWiringPi();

    int redPin = 7;
    int bluePin = 22;
    int iterations = 10;
    int delay = 1000;

    setPinMode(redPin, OUTPUT);
    setPinMode(bluePin, OUTPUT);

    for (int i = 1; i <= iterations; i++)
    {
        // Red LED
        printf("Red LED on - #%d\n", i);
        writeToPin(redPin, HIGH);
        wait(delay);
        printf("Red LED off - #%d\n", i);
        writeToPin(redPin, LOW);

        wait(delay);

        // Blue LED
        printf("Blue LED on - #%d\n", i);
        writeToPin(bluePin, HIGH);
        wait(delay);
        printf("Blue LED off - #%d\n", i);
        writeToPin(bluePin, LOW);

        wait(delay);
    }

    return 0; 
}
*/