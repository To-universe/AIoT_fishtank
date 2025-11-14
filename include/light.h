#include "MQTT.h"
#include "Adafruit_NeoPixel.h"

#define PIN 5                        // Define the pins of the RGB light
#define MAX_LED 50                   //64 RGB light

extern Adafruit_NeoPixel strip;
extern uint8_t i;                            
extern uint32_t color;   //绿、红、蓝

void light_test();
void light_mode1();
void light_close();
void light_norm();
void light_warning();
void light_error();
