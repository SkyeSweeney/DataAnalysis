
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "../msgs.h"


Body_t Body;

Msg_t Msg;

typedef struct BodyXyz_s
{
    struct aaa_s
    {
        struct bbb_s
        {
            union ccc_s
            {
                struct uu_s
                {
                    uint32_t p1:10;
                    uint32_t p2:10;
                    uint32_t p3:10;
                    uint32_t p4:2;
                } uu;
                uint32_t reg;
            } ccc;
            uint32_t var[20][40][15];
        } bbb;
    } aaa;
} BodyXyz_t;

BodyXyz_t qqq[44];


int main(int argc, char *argv[])
{

    return 0;
}

