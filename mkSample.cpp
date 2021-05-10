#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#include "msgs.h"


int main(int argc, char *argv[])
{

    Msg_t          msgLoc;
    uint32_t       i;
    FILE          *fout;

    double         northing    = 0.0;
    double         easting     = 0.0;
    double         altitude    = 0.0;
    double         speed       = 0.0;
    double         heading     = 0.0;

    uint32_t       sec = 0;
    uint32_t       nsec = 0;


    fout = fopen("sample.log", "wb");


    for (i=0; i<10000; i++)
    {
        

        msgLoc.hdr.SOM        = MSG_SOM;
        msgLoc.hdr.msgId      = MSGID_LOCATION;
        msgLoc.hdr.source     = NODE_ROBOT;
        msgLoc.hdr.length     = sizeof(BodyLocation_t);
        msgLoc.hdr.sec        = sec;
        msgLoc.hdr.nsec       = nsec;
        msgLoc.body.location.northing = northing;
        msgLoc.body.location.easting  = easting;
        msgLoc.body.location.altitude = altitude;
        msgLoc.body.location.speed    = speed;
        msgLoc.body.location.heading  = heading;

        fwrite(&msgLoc, sizeof(BodyLocation_t) + sizeof(MsgHeader_t), 1, fout);

        northing += (double)rand()/(double)(RAND_MAX) * 2.0 - 2.0/2.0;
        easting  += (double)rand()/(double)(RAND_MAX) * 2.0 - 2.0/2.0;
        altitude += (double)rand()/(double)(RAND_MAX) * 0.1 - 0.10/2.0;
        speed    += (double)rand()/(double)(RAND_MAX) * 1.0 - 1.00/2.0;
        heading  += (double)rand()/(double)(RAND_MAX) * 0.1 - 0.10/2.0;

        if (heading > 2.0*M_PI) 
        {
            heading -= 2.0*M_PI;
        }
        if (heading < 0.0) heading += 2.0*M_PI;

        nsec += 33333333;
        if (nsec > 1000000000)
        {
            nsec -= 1000000000;
            sec += 1;
        }

        printf("."); fflush(stdout);
    }
}


