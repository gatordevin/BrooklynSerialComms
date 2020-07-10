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
        packet recv_packet;
        BrooklynSerialComms(int baud);
        void begin();
        void send();
        int check_for_conn(int handshake_byte);
        void check_for_data(void(*on_data_callback)(packet data), void(*no_data_callback)());
};