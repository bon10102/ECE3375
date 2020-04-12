#include "address_map_arm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//initialize hardware pointers
volatile int *pbPtr = (int *)KEY_BASE;
volatile int *swPtr = (int *)SW_BASE;
volatile int *dispPtr1 = (int *)HEX3_HEX0_BASE;
volatile int *dispPtr2 = (int *)HEX5_HEX4_BASE;
volatile int *countPtr = (int *)TIMER_BASE;
//count time of 1 hundreths of a second
const int countTime = 10; 
const int hexValues[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
//longitude values of the track
const float longi[24] = {40.84679, -40.84768, 40.848075, 40.849038, 40.850758, 40.851704, 40.852615, 40.853509, 40.854455, 40.855384, 40.8547, 40.853982, 40.853314,
                         40.852612, 40.851877, 40.851225, 40.850474, 40.849972, 40.849271, 40.848719, 40.848151, 40.847567, 40.846949, 40.846731};
//latitude values of the track
const float lat[24] = {-96.761682, -96.761725, -96.761657, -96.761634, -96.761634, -96.761657, -96.761566, -96.761588, -96.761679, -96.761634, -96.760793, -96.760086,
                       -96.759335, -96.758606, -96.757789, -96.757082, -96.756376, -96.756817, -96.757701, -96.758606, -96.759512, -96.760395, -96.761367, -96.761654};

/**
 * Counts one hundreths of a second
 */
void timer()
{
    *(countPtr + 3) = countTime; //counter start value (high)
    *(countPtr + 2) = 0;         //counter start value (low)
    *(countPtr + 1) = 0x4;       //control register. 0x4 = start, 0x8 = stop
    while ((*(countPtr)&0x01) != 1)
    {
    } //reset the "TO" bit
    *(countPtr) = 0x0;
}

/**
 * Reads the pushbutton bank
 * @return the state of the pushbuttons
 */
int readPb()
{
    return *(pbPtr);
}

/**
 * Reads the first 3 switches of the switch bank
 * @return the state of the first 3 pushbuttons
 */
int readSwitches()
{
    return *(swPtr)&0x7; //mask with '111' because we only care about the 1st switch
}

/**
 * Concatenates an array of 6 integers into 1
 * @param arr the array of integers to be concatenated
 * @return the formed integer
 */
int concat(volatile int arr[])
{
    char s1[20];
    char s2[20];
    char s3[20];
    char s4[20];
    char s5[20];
    char s6[20];
    // Convert integers to string
    sprintf(s1, "%d", arr[0]);
    sprintf(s2, "%d", arr[1]);
    sprintf(s3, "%d", arr[2]);
    sprintf(s4, "%d", arr[3]);
    sprintf(s5, "%d", arr[4]);
    sprintf(s6, "%d", arr[5]);

    // Concatenate strings
    strcat(s1, s2);
    strcat(s1, s3);
    strcat(s1, s4);
    strcat(s1, s5);
    strcat(s1, s6);

    // Convert the concatenated string
    // to integer
    int c = atoi(s1);

    // return the formed integer
    return c;
}

int main()
{
    //the CSV read would occur here
    //seed random
    srand(time(NULL));
    volatile int random = 0;
    volatile unsigned int lapNumber = 0;
    volatile int hundreths = 0, tenths = 0, seconds1 = 0, seconds2 = 0, minutes1 = 0, minutes2 = 0;
    volatile int startOrStop = 0;
    volatile int lapTime1 = (int)NULL;
    volatile int lapTime2 = (int)NULL;
    const float startOfTrackLong = longi[0];
    const float startOfTrackLat = lat[0];
    volatile int currentLapTime[6];
    volatile int fastestTime = 0;
    volatile int fastestTimeHex1 = (int)NULL;
    volatile int fastestTimeHex2 = (int)NULL;
    volatile unsigned int position = 0;
    while (1)
    {
        random = rand() % 200; //return random number between 0 and 200
        //these if statements simulate the car driving down the track
        if (random == 1 && position < 23)
        {
            position += 1;
        }
        if (position == 23){
            position = 0;
        }

        //determines when the timer display should roll over to next higher order
        if (hundreths > 9)
        {
            hundreths = 0;
            tenths += 1;
        }
        else if (tenths > 9)
        {
            tenths = 0;
            seconds1 += 1;
        }
        else if (seconds1 > 9)
        {
            seconds1 = 0;
            seconds2 += 1;
        }
        else if (seconds2 > 5)
        {
            seconds2 = 0;
            minutes1 += 1;
        }
        else if (minutes1 > 9)
        {
            minutes1 = 0;
            minutes2 += 1;
        }
        else if (minutes2 > 5) //if the timer reaches max count, reset to 0
        {
            hundreths = 0, tenths = 0, seconds1 = 0, seconds2 = 0, minutes1 = 0, minutes2 = 0;
        }

        //controls operation of stopwatch
        switch (readPb())
        {
        case 0x1: //start the stopwatch
            startOrStop = 1;
            break;
        case 0x2: //stop the stopwatch
            startOrStop = 0;
            break;
        case 0x4: //reset the stopwatch
            hundreths = 0, tenths = 0, seconds1 = 0, seconds2 = 0, minutes1 = 0, minutes2 = 0;
            lapTime1 = (int)NULL;
            lapTime2 = (int)NULL;
            lapNumber = 0;
            fastestTime = 0;
            fastestTimeHex1 = (int)NULL;
            fastestTimeHex2 = (int)NULL;
            break;
        }

        //controls which information should be displayed on the displays
        switch(readSwitches()){
            case 0x0: //display the current time
                *(dispPtr1) = hexValues[hundreths] + 0x100 * hexValues[tenths] + 0x10000 * hexValues[seconds1] + 0x1000000 * hexValues[seconds2];
                *(dispPtr2) = hexValues[minutes1] + 0x100 * hexValues[minutes2];
                break;
            case 0x1: //display the current lap time
                *(dispPtr1) = lapTime1;
                *(dispPtr2) = lapTime2;
                break;
            case 0x2: //display fastest lap time
                *(dispPtr1) = fastestTimeHex1;
                *(dispPtr2) = fastestTimeHex2;
                break;
            case 0x4: //display number of laps, up to 999
                *(dispPtr1) = hexValues[lapNumber % 10] + 0x100 * hexValues[(lapNumber / 10) % 10] + 0x10000 * hexValues[(lapNumber / 100) % 10];
                *(dispPtr2) = (int)NULL;
                break;
        }

        //counts a lap, set lap and fastest times, and resets current time when a lap is completed
        if (longi[position] == startOfTrackLong && lat[position] == startOfTrackLat && startOrStop == 1)
        {
            //store the lap time
            lapTime1 = hexValues[hundreths] + 0x100 * hexValues[tenths] + 0x10000 * hexValues[seconds1] + 0x1000000 * hexValues[seconds2];
            lapTime2 = hexValues[minutes1] + 0x100 * hexValues[minutes2];
            currentLapTime[0] = minutes2;
            currentLapTime[1] = minutes1;
            currentLapTime[2] = seconds2;
            currentLapTime[3] = seconds1;
            currentLapTime[4] = tenths;
            currentLapTime[5] = hundreths;
            //updates fastest lap time if the most recent lap was the fastest
            if (fastestTime > concat(currentLapTime) || lapNumber == 0)
            {
                fastestTime = concat(currentLapTime); //assign the new fastest time
                fastestTimeHex1 = hexValues[hundreths] + 0x100 * hexValues[tenths] + 0x10000 * hexValues[seconds1] + 0x1000000 * hexValues[seconds2];
                fastestTimeHex2 = hexValues[minutes1] + 0x100 * hexValues[minutes2];
            }
            lapNumber += 1; //increase the lap counter
            position = 1; //reset the position
            hundreths = 0, tenths = 0, seconds1 = 0, seconds2 = 0, minutes1 = 0, minutes2 = 0;
        }

        //reset the number of laps when it has gone past 999
        if (lapNumber >= 999)
        {
            lapNumber = 0;
        }

        //counts a hundreth of a second and increment the current time
        timer();
        if (startOrStop == 1)
        {
            hundreths += 1;
        }
        else
        {
            hundreths += 0;
        }
    }
}
