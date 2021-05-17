#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#include "msgs.h"

// Drive in a circle of radius 14 meters at 4m/s
// at center 19T 296840E 4733202E


int main(int argc, char *argv[])
{

    Msg_t          msgLoc;
    FILE          *fout;

    double         northing;
    double         easting;
    double         altitude    = 0.0;
    double         speed;
    double         heading;

    uint32_t       sec = 0;
    uint32_t       nsec = 0;

    double         r = 14.0;
    double         vel = 4.0;
    double         centerE = 296840.0;
    double         centerN = 4733202.0;
    double         a = 0.0;
    double         dt = 1/30.0;
    double         omega = vel/r;


    fout = fopen("sample.log", "wb");


    for (;;)
    {
        
        // Radial angle
        a += omega * dt;

        // Position
        easting = centerE + r * cos(a);
        northing = centerN + r * sin(a);

        altitude = 0.0;

        // velocity
        speed = vel;

        // Heading
        heading = 2*M_PI - a;
        if (heading > 2.0*M_PI) 
        {
            heading -= 2.0*M_PI;
        } else if (heading < 0.0) 
        {
            heading += 2.0*M_PI;
        }

        // Time
        nsec += dt * 1000000000.0; // convert to nsec
        if (nsec > 1000000000)
        {
            nsec -= 1000000000;
            sec += 1;
        }

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

        if (sec > 5*60) break;

    }
}


