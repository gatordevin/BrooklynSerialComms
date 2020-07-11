#include <Arduino.h>

struct packet
{
    byte header;
    
    byte destination_id;

    byte sender_id;

    unsigned long packet_id;

    byte command;

    byte data_length;

    signed long checksum;

    byte data[50];
};

union four_byte_conv {
    byte b[4];
    float fval;
    unsigned int uival;
    int ival;
};

union two_byte_conv {
    byte b[2];
    unsigned short int usval;
    short int sval;
};

union eight_byte_conv {
    byte b[8];
    unsigned long long ulval;
    long long lval;
};

class BrooklynSerialComms
{
    unsigned int _baud;
    unsigned short _watchdog_timer;
    unsigned short _ser_connect_time;
    unsigned long _packets_recieved;
    unsigned int _header_length_errors;
    unsigned int _data_length_errors;
    unsigned int _checksum_errors;
    unsigned int _watchdog_errors;
    unsigned short _starting_pos;

    public:
        
        BrooklynSerialComms(unsigned int baud, short watchdog_timer);
        void begin();
        void send_full_packet();
        void send_byte(byte value, byte destination_id);
        void send_metrics(byte destination_id);
        void close(void(*end_comms_handler)());
        float read_float();
        unsigned int read_unsigned_int();
        int read_int();
        unsigned short read_unsigned_short();
        short read_short();
        long long read_long_long();
        unsigned long long read_unsigned_long_long();
        int check_for_conn(unsigned short handshake_byte);
        void check_for_data(void(*on_data_callback)(packet data), void(*no_data_callback)(), void(*watchdog_handler)());
        packet recv_packet;
        packet send_packet;
        four_byte_conv four_byte_converter;
        two_byte_conv two_byte_converter;
        eight_byte_conv eight_byte_converter;
        
    private:
        int _last_recv_time;
        int _current_time;
};