#include <BrooklynSerialComms.h>

BrooklynSerialComms::BrooklynSerialComms(int baud)
{
    _baud = baud;
}

void BrooklynSerialComms::begin()
{
    Serial.begin(_baud);
    Serial.setTimeout(250);
}

void BrooklynSerialComms::send()
{
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    Serial.write(byte_array, sizeof(send_packet));
}