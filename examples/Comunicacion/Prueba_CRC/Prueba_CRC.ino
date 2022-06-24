void setup() {
  Serial.begin(115200);
  Serial.println(crc8(0xFAFAFA, 3, 0x07), HEX);
}

void loop() {

}

uint8_t crc8(uint8_t const message[], unsigned nBytes, uint8_t polynomial) {
    uint8_t remainder = 0;  
    unsigned byte, bit;

    for (byte = 0; byte < nBytes; ++byte) {
        remainder ^= message[byte];
        for (bit = 0; bit < 8; ++bit) {
            if (remainder & 0x80) {
                remainder = (remainder << 1) ^ polynomial;
            }
            else {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}
