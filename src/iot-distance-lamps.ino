/*
 * IOT Distance Lamps
 * Wirelessly connected LED lamps that synchronize brightness and color from anywhere in the world
 * Author: Brandon Liu
 * Date: April 29, 2020
 */
#include "neopixel.h"
#include <string>
#define BUTTON D0

#define BRIGHTNESS_LEVEL 50 // 0 - 255
#define COLOR_TRANSITION_DELAY 25 // Total delay in ms

#define PIXEL_PIN D2
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
uint32_t Wheel(uint8_t position);
void fill(Adafruit_NeoPixel* strip, uint32_t color);

#define NUM_COLORS 6
int currColor = 0;
unsigned long colorPalette[NUM_COLORS] = {
  // strip.Color(255, 0, 0),    // red
  // strip.Color(0, 255, 0),    // green
  // strip.Color(0, 0, 255),    // blue
  // strip.Color(255, 255, 0),  // yellow
  // strip.Color(80, 0, 80),    // purple
  // strip.Color(0, 255, 255)  // aqua
  strip.Color(26, 188, 156),
  strip.Color(155, 89, 182),
  strip.Color(192, 57, 43),
  strip.Color(218, 247, 166),
  strip.Color(41, 128, 185),
  strip.Color(255, 195, 0),
};

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
  Particle.function("next-color", nextColor);
  Particle.function("set-brightness", mySetBrightness);
  // set up passive pull-up,
  // when button is pressed BUTTON_PIN will read LOW since it's connected to ground
  pinMode(BUTTON, INPUT_PULLUP);
  digitalWrite(BUTTON, HIGH);
  Serial.begin(9600);


  strip.begin();
  strip.setBrightness(BRIGHTNESS_LEVEL);
  strip.show();
  fill(&strip, colorPalette[currColor]);
  strip.show();
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
  int nextColor = ((currColor + 1) % NUM_COLORS);
  transition(&strip, colorPalette[currColor], colorPalette[nextColor]);
  currColor = nextColor;
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

// Smoothly transition between two colors by calculating
// the 3D vector between their RGB values and traversing it
void transition(Adafruit_NeoPixel* strip, uint32_t startingColor, uint32_t endingColor) {
  // Starting RGB values
  uint8_t sr = startingColor >> 16;
  uint8_t sg = startingColor >> 8;
  uint8_t sb = startingColor;
  // Ending RGB values
  uint8_t er = endingColor >> 16;
  uint8_t eg = endingColor >> 8;
  uint8_t eb = endingColor;

  while(sr != er || sg != eg || sb != eb) {
    if ( sr < er ) sr += 1;
    if ( sr > er ) sr -= 1;
    if ( sg < eg ) sg += 1;
    if ( sg > eg ) sg -= 1;
    if ( sb < eb ) sb += 1;
    if ( sb > eb ) sb -= 1;
    fill(strip, strip->Color(sr, sg, sb));
    strip->show();
  }

  // Delta RGB values ("slope")
  // int steps = 50;
  // double dr = (sr-er)/steps;
  // double dg = (sg-eg)/steps;
  // double db = (sb-eb)/steps;
  // uint32_t currColor = startingColor;
  // for(int i = 0; i < steps; i++) {
  //   fill(strip, currColor);
  //   strip->show();
  //   currColor = strip->Color((uint8_t)(sr+=dr), (uint8_t)(sg+=dg), (uint8_t)(sb+=db));
  //   delay(10);
  // }
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
