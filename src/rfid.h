#ifndef RFID_H
#define RFID_H

#include <main.h>

void rfid_init();
void rfid_loop();
String read_tag_re();
bool cmd_stop_inventory();
bool cmd_inventory_epc();
bool cmd_read_tag(uint8_t ant);
bool cmd_get_firmware_version();

#endif // !RFID_H