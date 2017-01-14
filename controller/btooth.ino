#include "btooth.h"

void setup_btooth(){
  btooth_serial.begin(BTOOTH_RATE);
}

String read_btooth_data(){
  if (btooth_serial.available()) {
    rcv_buf = "";
    while(btooth_serial.available()) {
      rcv_buf += (char)btooth_serial.read();
    }
    return rcv_buf;
  }
  return "";
}

void write_btooth_data(String data){
    btooth_serial.println(data);
}

