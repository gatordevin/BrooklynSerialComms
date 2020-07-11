#include <Arduino.h>

struct packet
{
    byte header;
    
    byte destination_id;

    byte sender_id;

    unsigned long packet_id;

    byte command;

    byte data_length;

    unsigned short checksum;

    byte data[50];
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

    public:
        
        BrooklynSerialComms(unsigned int baud, short watchdog_timer);
        void begin();
        void send_full_packet();
        void send_byte(byte value, byte destination_id);
        void send_metrics(byte destination_id);
        void close(void(*end_comms_handler)());
        int check_for_conn(unsigned short handshake_byte);
        void check_for_data(void(*on_data_callback)(packet data), void(*no_data_callback)(), void(*watchdog_handler)());
        packet recv_packet;
        packet send_packet;
        
    private:
        int _last_recv_time;
        int _current_time;
};