#include <BrooklynSerialComms.h>

BrooklynSerialComms::BrooklynSerialComms(int baud)
{
    _baud = baud;
}

void BrooklynSerialComms::begin()
{
    Serial.begin(_baud);
    Serial.setTimeout(100);
    while (!Serial) {}
}

void BrooklynSerialComms::send()
{
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    if(Serial.availableForWrite()>sizeof(send_packet)){
        Serial.write(byte_array, sizeof(send_packet));
        Serial.flush();
    }
}

int BrooklynSerialComms::check_for_conn(int handshake_byte)
{
    if(Serial.available()){
        int msg = Serial.read();
        if(msg == handshake_byte){
            if(Serial.availableForWrite()){
                Serial.write(msg);
                Serial.flush();
                return msg;
            }
        }else{
            return msg;
        }
        
    }else{
        return 0;
    }
    
}

void BrooklynSerialComms::check_for_data(void(*on_data_callback)(packet recv_packet), void(*no_data_callback)()){
    if(Serial.available()){
        if(Serial.readBytes((char*)&recv_packet, sizeof(packet))==sizeof(packet)){ //DATA LENGTH CHECK
            if(recv_packet.header!=255){
                recv_packet.data = 160; //HEADER CHECK
            }
            on_data_callback(recv_packet);
        }
        
    }else{
        no_data_callback();
    }
}