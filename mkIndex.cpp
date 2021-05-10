#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#include "msgs.h"

typedef struct IndexEntry_s
{
    long    pos;
    MsgId_t msgId;
} IndexEntry_t;


int main(int argc, char *argv[])
{

    Msg_t          msg;
    FILE          *fout;
    FILE          *fin;
    char           fn[256];
    char          *p;
    size_t         n;
    long           pos;
    IndexEntry_t   index;


    fin  = fopen(argv[1], "rb");
    strcpy(fn, argv[1]);
    p = strstr(fn, ".");
    p[0] = 0;
    strcat(fn, ".index");

    fout = fopen(fn, "wb");


    // Do till we run out of input
    for (; ;)
    {

        pos = ftell(fin);

        // Read the next header into the message buffer
        n = fread((void*)(&msg.hdr), sizeof(MsgHeader_t), 1, fin);

        // Did not read what we wanted
        if (n == 0)
        {
            if (feof(fin) == 0) 
            {
                printf("Error reading header.\n");
            }
            break;
        }

        // Verify header
        if (msg.hdr.SOM != 0x534b)
        {
            // Flush buffer and hope for the best
            printf("Invalid SOM.\n");
            break;
        }

        // If we have a body
        if (msg.hdr.length > 0)
        {

            // Read body
            n = fread((void*)(&msg.body), msg.hdr.length, 1, fin);

            if (n == 0)
            {
                printf("Error reading body.\n");
                break;
            }

        } // Have body

        // Write index info
        index.pos = pos;
        index.msgId = msg.hdr.msgId;
        fwrite(&index, sizeof(IndexEntry_t), 1, fout);
        printf("%ld %d\n", index.pos, index.msgId);

    }

    fclose(fin);
    fclose(fout);
}


