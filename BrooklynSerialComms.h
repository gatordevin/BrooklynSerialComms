#include <Arduino.h>

struct packet
{
    byte header = 255;
    byte data_len;
    byte data;
    byte stop = 120;
};

class BrooklynSerialComms
{
    int _baud;

    public:
        packet send_packet;
        BrooklynSerialComms(int baud);
        void begin();
        void send();
};