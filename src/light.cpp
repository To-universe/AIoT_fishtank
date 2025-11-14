#include "light.h"

unsigned long lasttime = 0;
uint8_t headIndex = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel( MAX_LED, PIN, NEO_RGB + NEO_KHZ800 );
uint8_t i = 0;                            
uint32_t color = strip.Color(75,156,215);   //绿、红、蓝

void light_test(){
    unsigned long now = millis();
    if(now - lasttime > 500){
        lasttime = now;
        strip.setPixelColor(headIndex,strip.Color(0,0,0));
        headIndex = (headIndex +1)%MAX_LED;
        strip.setPixelColor(headIndex,color);
        strip.show();
    }
}

void light_close(){
    for(int i = 0;i<MAX_LED;i++){
        strip.setPixelColor(i,strip.Color(0,0,0));
    }
    strip.show();
}
void light_norm(){
    for(int i =0;i<MAX_LED;i++){
        strip.setPixelColor(i,strip.Color(200,0,0));
    }
    strip.show();
}
void light_warning(){
    for(int i =0;i<MAX_LED;i++){
        strip.setPixelColor(i,strip.Color(150,200,0));
    }
    strip.show();
}
void light_error(){
    for(int i =0;i<MAX_LED;i++){
        strip.setPixelColor(i,strip.Color(0,200,0));
    }
    strip.show();
}

int n=0;
int x=0;
void light_mode1(){
    unsigned long now = millis();
    if(now -lasttime >20){
        lasttime = now;
        if(n ==0){
            for(int i =0;i<MAX_LED;i++){
                strip.setPixelColor(i,strip.Color(0,x,x));
            }
            strip.show();
            x+=1;
        }else if(n==1){
            for(int i =0;i<MAX_LED;i++){
                strip.setPixelColor(i,strip.Color(0,x,x));
            }
            strip.show();
            x-=1;
        }

        if(x>=200){
            n=1;
        }else if(x<=0){
            n=0;
        }
    }
}