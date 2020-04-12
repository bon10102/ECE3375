#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fPtr;
    fpos_t pos;
    fPtr = fopen("Coordinates.csv", "r");

    //array to store up to 24 lines of characters
    char singleLine[24][150];
    //parsed longitude data
    float longi[24];
    //parsed latitiude data
    float lat[24];

    //continue to read file pointer until end of file
    while (!feof(fPtr))
    {
        long int pos = 0;
        //reads file line by line
        for (int i = 0; i < 24; i++)
        {
            //determines which line to read of file
            fseek(fPtr, 0, pos);
            //read line and store it
            fgets(singleLine[i], 150, fPtr);
            //update file pointer (by telling it where the next line is)
            pos = ftell(fPtr);
            //print every character of each line to console
            puts(singleLine[i]);
        }
    }
    //parses the read data by only keeping float values
    for (int i = 0; i < 24; i++)
    {
        char tempLong[15];
        char tempLat[15];
        int j;
        for (j = 0; singleLine[i][j] != ','; j++)
        {
            tempLong[j] = singleLine[i][j];
        }
        for (int k = j+1; singleLine[i][k] != ','; k++){
            tempLat[k-j-1] = singleLine[i][k];
        }
        //convert cahracter array to a float
        longi[i] = strtof(tempLong, NULL);
        lat[i] = strtof(tempLat, NULL);
    }
    fclose(fPtr);
    //prints the first parsed coordinate set to console
    printf("%2.6f\n", longi[0]);
    printf("%2.6f\n", lat[0]);
    return 0;
}
