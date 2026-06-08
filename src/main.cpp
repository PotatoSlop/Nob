#include <Arduino.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <OneButton.h>

// ----------------------------- Encoder + button -----------------------------
#define DIAL_A_PIN 1
#define DIAL_B_PIN 0
#define BTN_PIN    3

OneButton btn = OneButton(BTN_PIN, true);

// Ben Buxton's bounce-immune quadrature state table (full step).
// It only emits a step when a COMPLETE valid A/B sequence is walked; contact
// bounce produces invalid transitions that rattle between states and emit
// nothing -- so no phantom jumps, reversals, or dropped detents.
// 4x quadrature decoder: counts every valid A/B transition (= one per detent on
// this Gray-per-click encoder). The lookup gives +1/-1 for legal steps and 0 for
// illegal jumps; a bounce that flips back then forward nets to zero -> self-correcting.
// Index = (prevAB << 2) | curAB.
static const int8_t qtable[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

const int ROTARY_MIN = 0;
const int ROTARY_MAX = 100;
const int DETENT_DIV = 2;   // raw transitions per detent (tune: 1, 2, or 4)
int     rotaryPos = 0;
int     rawAccum  = 0;
uint8_t prevAB    = 0;

// Polled (NOT an interrupt) — call often from loop(). Decodes every transition
// (bounce-tolerant), then emits one detent step per DETENT_DIV transitions.
void pollEncoder() {
  uint8_t curAB = (digitalRead(DIAL_A_PIN) << 1) | digitalRead(DIAL_B_PIN);
  if (curAB != prevAB) {
    rawAccum += qtable[(prevAB << 2) | curAB];
    prevAB = curAB;
    while (rawAccum >= DETENT_DIV)  { rawAccum -= DETENT_DIV; if (rotaryPos < ROTARY_MAX) rotaryPos++; }
    while (rawAccum <= -DETENT_DIV) { rawAccum += DETENT_DIV; if (rotaryPos > ROTARY_MIN) rotaryPos--; }
  }
}

// --------------------------- Display: GC9A01 240x240 ------------------------
// Wiring (display pin -> ESP32-C3 SuperMini GPIO):
//   SDA->GPIO6  SCL->GPIO7  DC->GPIO5  CS->GPIO4  RES->GPIO10
//   VCC->3V3    GND->GND    BLK->3V3
#define GFX_SCK  7
#define GFX_MOSI 6
#define GFX_MISO -1   // write-only panel
#define GFX_DC   5
#define GFX_CS   4
#define GFX_RST  10

const int SCREEN_W = 240;
const int SCREEN_H = 240;

#define COL_BLACK 0x0000
#define COL_WHITE 0xFFFF
#define COL_GREY  0x7BEF

Arduino_DataBus *bus = new Arduino_HWSPI(GFX_DC, GFX_CS, GFX_SCK, GFX_MOSI, GFX_MISO);
Arduino_GFX    *gfx = new Arduino_GC9A01(bus, GFX_RST, 0 /* rotation */, true /* IPS */);

void handleButtonClick();
void drawVolume(int v);

int lastShown = -1;

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("=== Nob booted ===");

  // Encoder: internal pull-ups (common pin -> GND). Polled in loop(), no ISR.
  pinMode(DIAL_A_PIN, INPUT_PULLUP);
  pinMode(DIAL_B_PIN, INPUT_PULLUP);

  btn.attachClick(handleButtonClick);

  // Display
  gfx->begin();
  gfx->fillScreen(COL_BLACK);
  gfx->setTextColor(COL_GREY);
  gfx->setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  gfx->getTextBounds("VOLUME", 0, 0, &x1, &y1, &w, &h);
  gfx->setCursor((SCREEN_W - w) / 2, 50);
  gfx->print("VOLUME");

  lastShown = rotaryPos;
  drawVolume(lastShown);
}

void loop() {
  pollEncoder();
  int v = rotaryPos;

  if (v != lastShown) {
    lastShown = v;
    Serial.print("Val: ");
    Serial.println(v);
    drawVolume(v);
  }

  // Heartbeat so the monitor always shows life (and we can tell running vs crashed).
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    Serial.print("alive, vol=");
    Serial.println(v);
  }

  btn.tick();
}

void handleButtonClick() {
  Serial.println("Click");
}

// Big volume number, centred. Clears a band first, then centres via measured bounds.
void drawVolume(int v) {
  char buf[8];
  snprintf(buf, sizeof(buf), "%d", v);

  gfx->setTextSize(6);
  int16_t x1, y1;
  uint16_t w, h;
  gfx->getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  gfx->fillRect(0, SCREEN_H / 2 - 30, SCREEN_W, 60, COL_BLACK);
  gfx->setTextColor(COL_WHITE);
  gfx->setCursor((SCREEN_W - w) / 2, SCREEN_H / 2 - h / 2);
  gfx->print(buf);
}
