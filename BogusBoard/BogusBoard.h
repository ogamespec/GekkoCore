
#pragma once

#define RAM_SIZE 24*1024*1024
#define BOOTROM_SIZE 1024*1024

#define RAM_START_ADDRESS 0
#define BOOTROM_START_ADDRESS 0xfff0'0000

#define UNMAPPED_VALUE 0

void HWInit();
void HWShutdown();

extern uint8_t* RAM;
extern uint8_t* BootROM;
