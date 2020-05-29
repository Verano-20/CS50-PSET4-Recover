#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool check_signature(unsigned char *buffer);

int main(int argc, char *argv[])
{
    // check for correct number of command line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // initialise buffer
    unsigned char *buffer = (unsigned char *)malloc(512 * sizeof(unsigned char)); // allocate memory for the buffer

    // initialise counters for JPEGs recovered and blocks checked
    int jpegs = 0, blocks = 0;

    // open file to be recovered
    FILE *file = fopen(argv[1], "r");

    // initialise img for outfile
    FILE *img;

    // initialise string for outfile name
    char *outfile = malloc(7 * sizeof(char));

    // while loop to stop at end of card
    while (fread(buffer, 1, 512, file) == 512)
    {
        // update block counter
        blocks++;
        // check for signature
        if (check_signature(buffer) == true)
        {
            // if first image, create new image and write to it
            if (jpegs < 1)
            {
                sprintf(outfile, "%03i.jpg", jpegs); // write file name to outfile string
                jpegs++; // add one to jpegs count
                img = fopen(outfile, "w"); // open (create) outfile
                fwrite(buffer, 512, 1, img); // write contents of buffer to outfile
            }
            // else, close previous file and create new jpg to write to
            else
            {
                fclose(img); // close current jpg
                sprintf(outfile, "%03i.jpg", jpegs); // write file name to outfile string
                jpegs++; // add one to jpegs count
                img = fopen(outfile, "w"); // open (create) outfile
                fwrite(buffer, 512, 1, img); // write contents of buffer to outfile
            }
        }
        // if there is no signature
        else
        {
            //check if there is already a jpeg being written
            if (jpegs > 0)
            {
                fwrite(buffer, 512, 1, img); // write contents of buffer to outfile
            }
        }
        //move to next block in file
        fseek(file, 512 * blocks, SEEK_SET);
    }
    free(buffer);
    free(outfile);
}


// function to check if a JPEG signature is found at the start of an array
bool check_signature(unsigned char *buffer)
{
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
    {
        return true;
    }
    return false;
}
