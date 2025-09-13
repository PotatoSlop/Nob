#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int POT_PIN = A0;
const int POT_MIN = 0;
const int POT_MAX = 1023;

int lastSentPercent = -1; // Stores latest % sent to volume control

// Custom characters for display volume bars
byte bar_1[8] = { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000 }; //8 rows of pixels, 5 total bars 
byte bar_2[8] = { 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000 };
byte bar_3[8] = { 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100 };
byte bar_4[8] = { 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110 };
byte bar_5[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, bar_1);
  lcd.createChar(1, bar_2);
  lcd.createChar(2, bar_3);
  lcd.createChar(3, bar_4);
  lcd.createChar(4, bar_5);
}

void loop() {
  int currentPotValue = analogRead(POT_PIN);
  
  // Map percentage values of volume to calibrated range of pot.
  int volumePercent = map(currentPotValue, POT_MIN, POT_MAX, 0, 100);
  volumePercent = constrain(volumePercent, 0, 100);

  if (volumePercent != lastSentPercent) {
    Serial.println(volumePercent); //Update vol. percentage --> python script
    lastSentPercent = volumePercent; 
  }

  // LCD Styling
  lcd.setCursor(0, 0);
  lcd.print("Volume: ");
  if (volumePercent < 10) lcd.print("  ");
  if (volumePercent >= 10 && volumePercent < 100) lcd.print(" ");
  lcd.print(volumePercent);
  lcd.print("%");
  drawVolumeBar(volumePercent);
  
  delay(50);
}

void drawVolumeBar(int percent) {
  lcd.setCursor(0, 1);
  int pixels = map(percent, 0, 100, 0, 80);
  int fullBlocks = pixels / 5;
  int partialPixels = pixels % 5;

  for (int i = 0; i < fullBlocks; i++) { 
    lcd.write(byte(4)); 
  }

  if (fullBlocks < 16 && partialPixels > 0) { 
    lcd.write(byte(partialPixels - 1)); 
  }
  int currentBarLength = fullBlocks + (partialPixels > 0 ? 1 : 0);

  for (int i = currentBarLength; i < 16; i++) {
    lcd.print(" "); 
  }
}