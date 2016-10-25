
#define BLUEFRUIT_UART_MODE_PIN        -1
#define FACTORYRESET_ENABLE      1

// Smartphone-activated audio gizmo (e.g. talking dog collar).
// Uses the following Adafruit parts:
//
// - Bluefruit LE Micro (adafruit.com/product/2661)
// - 500 mAh LiPoly battery (1578)
// - LiPoly backpack (2124)
// - Audio FX Mini sound board (2342 or 2341)
// - 2.5W class D mono amp (2130)
// - Speaker (TBD)
//
// Needs Adafruit_BluefruitLE_nRF51 and Adafruit_Soundboard libs:
// github.com/adafruit

#include <SPI.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_Soundboard.h>

#define LED         A0 // LED on while "talking"
#define AUDIO_ACT   5  // "Act" on Audio FX
#define AUDIO_RESET 6  // "Rst" on Audio FX

Adafruit_Soundboard      sfx(&Serial1, NULL, AUDIO_RESET);
Adafruit_BluefruitLE_SPI ble(8, 7, 4); // CS, IRQ, RST pins

char filename[12] = "        OGG"; // Tail end of filename NEVER changes

// PROGMEM string arrays are wretched, and sfx.playTrack() expects a
// goofy fixed-length space-padded filename...we take care of both by
// declaring all the filenames inside one big contiguous PROGMEM string
// (notice there are no commas here, it's all concatenated), and copying
// an 8-byte section as needed into filename[].  Some waste, but we're
// not hurting for space.  If you change or add any filenames, they MUST
// be padded with spaces to 8 characters, else there will be...trouble.
static const char PROGMEM bigStringTable[] =  // play() index
  "1       " "2       " "3       " "4       " //  0-3
  "5       " "6       " "7       " "8       " //  4-7
  "BOOT    ";                                 //  8-

void fail(uint16_t ms) { // If startup error, LED flash indicates status
  for(uint8_t x=0;;) {
    digitalWrite(LED, ++x & 1);
    Serial.print("hmmmm ");
    Serial.println(ms);
    delay(ms);
  }
}

void setup(void) {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);         // LED steady on during init
  Serial1.begin(9600);             // Audio FX serial link
  if(!sfx.reset())      fail(250); // Audio FX init error?  Slow blink
  if(!ble.begin(false)) fail(100); // BLE init error?  Fast blink
  ble.echo(false);
  digitalWrite(LED, LOW);          // LED off = successful init
  play(8);                         // Play startup sound
}

void loop(void) {
  if(ble.isConnected()) {
    ble.println(F("AT+BLEUARTRX"));     // Request string from BLE module
    ble.readline();                     // Read outcome
    Serial.print("received: ");
    Serial.println(ble.buffer);
    uint8_t track = ble.buffer[0] - '0';
    if (track < 9) {
      play(track);
    }
    ble.waitForOK();
  }
}

void play(uint16_t i) {
  digitalWrite(LED, HIGH);
  memcpy_P(filename, &bigStringTable[i * 8], 8); // PROGMEM -> RAM
  sfx.playTrack(filename);
  delay(250); // Need this -- some delay before ACT LED is valid
  while(digitalRead(AUDIO_ACT) == LOW); // Wait for sound to finish
  digitalWrite(LED, LOW);
}
