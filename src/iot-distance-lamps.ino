/*
 * IOT Distance Lamps
 * Wirelessly connected LED lamps that synchronize brightness and color from anywhere in the world
 * Author: Brandon Liu
 * Date: April 29, 2020
 */
#include "neopixel.h"
#include <string>
#define BUTTON D0

#define BRIGHTNESS_LEVEL 25 // 0 - 255
#define COLOR_TRANSITION_DELAY 25 // Total delay in ms

#define PIXEL_PIN D2
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
uint32_t Wheel(uint8_t position);
void fill(Adafruit_NeoPixel* strip, uint32_t color);

#define NUM_COLORS 6
enum colorName {
  teal,
  ocean,
  purple,
  warm_red,
  pale_green,
  warm_yellow
};

unsigned long colorPalette[NUM_COLORS] = {
  strip.Color(26, 188, 156),
  strip.Color(41, 128, 185),
  strip.Color(155, 89, 182),
  strip.Color(192, 57, 43),
  strip.Color(218, 247, 166),
  strip.Color(255, 195, 0),
};

colorName currColor = teal;
String touchEventName = "touch_event";
// bool pressing = false;
// unsigned int lastPress, lastRelease, previousLastPress, previousLastRelease;
//
// int buttonHandler()  // 0 = not pressing, 1 = still pressing, 2 = just stopped pressing, 3 = just started pressing
// {
//         if (digitalRead(BUTTON) == LOW && pressing == false) // 3 = just started pressing
//         {
//                 pressing = true;
//                 lastPress = millis();
//                 return 3;
//         }
//
//         if (digitalRead(BUTTON) == HIGH && pressing == true) // 2 = just stopped pressing
//         {
//                 pressing = false;
//                 lastRelease = millis();
//                 return 2;
//         }
//
//         if (pressing == true) // 1 = still pressing
//         {
//                 return 1;
//         }
//         else // 0 = not pressing
//         {
//                 return 0;
//         }
// }

void setup() {
  uint32_t testColor = strip.Color(10, 11, 12, 13);
  char str[50];
  sprintf(str, "%ld", strip.Color(10, 11, 12, 13));
  uint8_t b = testColor;
  uint8_t g = testColor >> 8;
  uint8_t r = testColor >> 16;
  sprintf(str, "r: %d, g: %d, b: %d", r, g, b);
  Particle.publish("test", str);
  bool success = Particle.function("next-color", nextColor);
  bool success1 = Particle.function("set-brightness", mySetBrightness);
  // set up passive pull-up,
  // when button is pressed BUTTON_PIN will read LOW since it's connected to ground
  pinMode(BUTTON, INPUT_PULLUP);
  digitalWrite(BUTTON, HIGH);
  Serial.begin(9600);


  strip.begin();
  strip.setBrightness(BRIGHTNESS_LEVEL);
  strip.show();
  nextColor("");
}

// // Variables will change:
// int ledState = HIGH;         // the current state of the output pin
// int buttonState;             // the current reading from the input pin
// int lastButtonState = LOW;   // the previous reading from the input pin
// unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
// unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
void loop() {
  // int check = buttonHandler(); // get press status
  // if (previousLastRelease != lastRelease) // Button bumped, pushed and then released at later time
  // {
  //         Particle.publish("click","Click took "+String(lastRelease - lastPress)+" ms");
  //         nextColor();
  //         previousLastRelease = lastRelease;
  // }
  // int buttonState = digitalRead(BUTTON_PIN);
  // if(buttonState == HIGH) { // button pressed
  //   Serial.println("Is high");
  //   nextColor();
  // }
}

int nextColor(String extra) {
  currColor = (colorName)((currColor + 1) % NUM_COLORS);
  fill(&strip, colorPalette[currColor]);
  strip.show();
  return 0;
}

// takes a string "0" - "100" which represents the percentage brightness
int mySetBrightness(String text) {
  uint8_t brightness = (uint8_t)((strtol(text, NULL, 10)/100.0)*255);
  fill(&strip, colorPalette[currColor]);
  strip.setBrightness(brightness);
  strip.show();
  return brightness;
}
// Smoothly transition between two colors (0-255) by going through the color wheel
void transition(Adafruit_NeoPixel* strip, uint8_t startingColor, uint8_t endingColor) {
  for(uint8_t currColor = startingColor+1; currColor <= endingColor; currColor = (currColor+1)%256) {
    fill(strip, Wheel(currColor));
    delay(COLOR_TRANSITION_DELAY);
  }
}

// Set all pixels to color
void fill(Adafruit_NeoPixel* strip, uint32_t color) {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    strip->setPixelColor(i, color);
  }
}

// Input values 0 to 255 to get color values that transition R->G->B. 0 and 255 are the same color.
// https://votecharlie.com/blog/2018/08/improved-color-wheel-function.html
unsigned long Wheel(uint8_t position) {
  uint8_t R = 0, G = 0, B = 0;
  if (position < 85) {
    R = sqrt32((1530 - 9 * position) * position);
    G = sqrt32(65025 - 9 * position * position);
  } else if (position < 170) {
    position -= 85;
    R = sqrt32(65025 - 9 * position * position);
    B = sqrt32((1530 - 9 * position) * position);
  } else {
    position -= 170;
    G = sqrt32((1530 - 9 * position) * position);
    B = sqrt32(65025 - 9 * position * position);
  }
  return strip.Color(R, G, B);
}

// Adapted from https://www.stm32duino.com/viewtopic.php?t=56#p8160
unsigned int sqrt32(unsigned long n) {
  unsigned int c = 0x8000;
  unsigned int g = 0x8000;
  while(true) {
    if(g*g > n)
      g ^= c;
    c >>= 1;
    if(c == 0)
      return g;
    g |= c;
  }
}
