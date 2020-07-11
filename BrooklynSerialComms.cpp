#include <BrooklynSerialComms.h>

BrooklynSerialComms::BrooklynSerialComms(unsigned int baud, short watchdog_timer)
{
    _baud = baud;
    _watchdog_timer = watchdog_timer;
    _packets_recieved = 0;
    _header_length_errors = 0;
    _checksum_errors = 0;
    _data_length_errors = 0;
    _watchdog_errors = 0;
}

void BrooklynSerialComms::begin()
{
    Serial.begin(_baud);
    Serial.setTimeout(100);
}

void BrooklynSerialComms::close(void(*end_comms_handler)()){
    _data_length_errors = 0;
    _packets_recieved = 0;
    _checksum_errors = 0;
    _header_length_errors = 0;
    end_comms_handler();
}

void BrooklynSerialComms::send_full_packet()
{
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    if(Serial.availableForWrite()>sizeof(packet)-sizeof(send_packet.data)){
        Serial.write(byte_array, sizeof(packet)-sizeof(send_packet.data)+send_packet.data_length);
        Serial.flush();
    }
}

void BrooklynSerialComms::send_byte(byte value, byte destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = 7;
    send_packet.data_length = 1;
    send_packet.checksum = 0;
    send_packet.data[0] = value;
    send_full_packet();

}

void BrooklynSerialComms::send_metrics(byte destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    // send_packet.packed_id_1 = recv_packet.packed_id_1;
    // send_packet.packed_id_2 = recv_packet.packed_id_2;
    // send_packet.packed_id_3 = recv_packet.packed_id_3;
    // send_packet.packed_id_4 = recv_packet.packed_id_4;
    send_packet.command = 9;
    send_packet.data_length = 12;
    send_packet.checksum = 0;
    send_packet.data[0] = _packets_recieved;
    send_packet.data[1] = (_packets_recieved >> 8) & 0xFF;
    send_packet.data[2] = (_packets_recieved >> 16) & 0xFF;
    send_packet.data[3] = (_packets_recieved >> 24) & 0xFF;
    send_packet.data[4] = (byte) _header_length_errors;
    send_packet.data[5] = (byte) _header_length_errors >> 8;
    send_packet.data[6] = (byte) _checksum_errors;
    send_packet.data[7] = (byte) _checksum_errors >> 8;
    send_packet.data[8] = (byte) _data_length_errors;
    send_packet.data[9] = (byte) _data_length_errors >> 8;
    send_packet.data[10] = (byte) _watchdog_errors;
    send_packet.data[11] = (byte) _watchdog_errors >> 8;
    send_full_packet();
}

int BrooklynSerialComms::check_for_conn(unsigned short handshake_byte)
{
    if(Serial.available()){
        int msg = Serial.read();
        if(msg == handshake_byte){
            if(Serial.availableForWrite()){
                Serial.write(msg);
                Serial.flush();
                _ser_connect_time = millis();
                return msg;
            }
        }else{
            return msg;
        }
        
    }else{
        return 0;
    }
    
}

void BrooklynSerialComms::check_for_data(void(*on_data_callback)(packet recv_packet), void(*no_data_callback)(), void(*watchdog_handler)()){
    _current_time = millis();
    if(_current_time-_ser_connect_time<(_watchdog_timer*2) || _packets_recieved>0){
        if(_current_time-_last_recv_time<_watchdog_timer || _packets_recieved==0){
            if(Serial.available()){
                _last_recv_time  = _current_time;
                if(Serial.readBytes((char*)&recv_packet, sizeof(packet)-sizeof(recv_packet.data))==sizeof(packet)-sizeof(recv_packet.data)){
                    if(Serial.readBytes((char*)&recv_packet.data, recv_packet.data_length)==recv_packet.data_length){
                        _packets_recieved += 1;
                        on_data_callback(recv_packet);
                        no_data_callback();
                    }else{
                        _data_length_errors += 1;
                    }
                }else{
                    _header_length_errors += 1;
                }
            }else{
                no_data_callback();
            }
        }else{
            _data_length_errors = 0;
            _packets_recieved = 0;
            _checksum_errors = 0;
            _header_length_errors = 0;
            _watchdog_errors += 1;
            watchdog_handler();
        }
    }else{
        _data_length_errors = 0;
        _packets_recieved = 0;
        _checksum_errors = 0;
        _header_length_errors = 0;
        _watchdog_errors += 1;
        watchdog_handler();
    }
}