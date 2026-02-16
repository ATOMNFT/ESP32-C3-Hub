/*
 Adapted from the Adafruit and Xark's PDQ graphicstest sketch.

 This sketch is adjusted for:
 - ESP32-C3 Super Mini
 - ST7789 240x240 (GMT130-V1.0)
 - TFT_eSPI
 - Backlight on GPIO10 using LEDC PWM (ESP32 style)

 IMPORTANT:
 - In your TFT_eSPI User_Setup.h (or chosen setup), set:
   #define ST7789_DRIVER
   #define TFT_WIDTH  240
   #define TFT_HEIGHT 240
   #define TFT_BL 10            // <-- your BL pin
   #define TFT_DC 1
   #define TFT_RST 2
   #define TFT_MISO 5
   #define TFT_MOSI 6
   #define TFT_SCLK 4
   #define TFT_CS   -1
*/

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Backlight control (uses TFT_BL if defined, else disables PWM)
#if defined(TFT_BL) && (TFT_BL >= 0)
  const int pwmPin     = TFT_BL;   // e.g. GPIO10
  const int BL_PWM_CH  = 0;
  const int BL_PWM_FREQ= 5000;
  const int BL_PWM_RES = 8;        // 0-255
  #define HAS_BL_PWM 1
#else
  #define HAS_BL_PWM 0
#endif

// Forward declarations
void printnice(int32_t v);
static inline uint32_t micros_start() __attribute__ ((always_inline));
static inline uint32_t micros_start();

uint32_t testHaD();
uint32_t testFillScreen();
uint32_t testText();
uint32_t testPixels();
uint32_t testLines(uint16_t color);
uint32_t testFastLines(uint16_t color1, uint16_t color2);
uint32_t testRects(uint16_t color);
uint32_t testFilledRects(uint16_t color1, uint16_t color2);
uint32_t testFilledCircles(uint8_t radius, uint16_t color);
uint32_t testCircles(uint8_t radius, uint16_t color);
uint32_t testTriangles();
uint32_t testFilledTriangles();
uint32_t testRoundRects();
uint32_t testFilledRoundRects();

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Bodmer's TFT_eSPI library Test! (ESP32-C3 + ST7789 240x240)");

  tft.init();
  tft.setRotation(0); // change if needed: 0/1/2/3

#if HAS_BL_PWM
  ledcSetup(BL_PWM_CH, BL_PWM_FREQ, BL_PWM_RES);
  ledcAttachPin(pwmPin, BL_PWM_CH);
  ledcWrite(BL_PWM_CH, 255);
#endif

  // Quick backlight dim demo
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(20, 110);
  tft.setTextSize(2);
  tft.print(F("Backlight Dimming"));
  delay(500);

#if HAS_BL_PWM
  for (int i = 255; i >= 0; i--) { ledcWrite(BL_PWM_CH, i); delay(5); }
  for (int i = 0; i <= 128; i++) { ledcWrite(BL_PWM_CH, i); delay(5); }
  delay(250);
  ledcWrite(BL_PWM_CH, 255);
#else
  delay(500);
#endif

  tft.fillScreen(TFT_BLACK);
}

void loop(void)
{
  Serial.println();
  Serial.print(F("Processor : "));
  #if defined(CONFIG_IDF_TARGET_ESP32S3)
    Serial.println("ESP32-S3");
  #elif defined(CONFIG_IDF_TARGET_ESP32S2)
    Serial.println("ESP32-S2");
  #elif defined(CONFIG_IDF_TARGET_ESP32C3)
    Serial.println("ESP32-C3");
  #elif defined (ESP32)
    Serial.println("ESP32");
  #elif defined (ARDUINO_ARCH_ESP8266)
    Serial.println("ESP8266");
  #elif defined (STM32)
    Serial.println("STM32");
  #elif defined(ARDUINO_ARCH_RP2040)
    Serial.println("RP2040");
  #elif defined(__LGT8FX8P__)
    Serial.println("LGT8FX");
  #else
    Serial.println("Arduino");
  #endif

  Serial.print(F("ESP32     : "));
  Serial.print(ESP_ARDUINO_VERSION_MAJOR);
  Serial.print(".");
  Serial.print(ESP_ARDUINO_VERSION_MINOR);
  Serial.print(".");
  Serial.print(ESP_ARDUINO_VERSION_PATCH);
  Serial.println();

  Serial.print(F("TFT_eSPI  : "));
  Serial.println(TFT_ESPI_VERSION);

  Serial.print(F("SPI Write : "));
  Serial.print(SPI_FREQUENCY/1000000);
  Serial.println(F(" MHz"));

  #if defined(USE_HSPI_PORT)
    Serial.println(F("#define USE_HSPI_PORT"));
  #elif defined(USE_VSPI_PORT)
    Serial.println(F("#define USE_VSPI_PORT"));
  #elif defined(USE_FSPI_PORT)
    Serial.println(F("#define USE_FSPI_PORT"));
  #else
    Serial.println(F("no #define USE_xSPI_PORT"));
  #endif

  Serial.print(F("MOSI ")); Serial.print(TFT_MOSI);
  if (TFT_MOSI != MOSI) { Serial.print(F("/")); Serial.print(MOSI); }

  Serial.print(F(" - MISO ")); Serial.print(TFT_MISO);
  if (TFT_MISO != MISO) { Serial.print(F("/")); Serial.print(MISO); }

  Serial.print(F(" - SCLK ")); Serial.print(TFT_SCLK);
  if (TFT_SCLK != SCK) { Serial.print(F("/")); Serial.print(SCK); }

  Serial.print(F(" - CS  ")); Serial.print(TFT_CS);
  if (TFT_CS != SS) { Serial.print(F("/")); Serial.print(SS); }

  Serial.print(F(" - DC  ")); Serial.print(TFT_DC);
  Serial.print(F(" - RST ")); Serial.println(TFT_RST);

  Serial.println();
  Serial.println(F("Benchmark                Time (microseconds)"));

  uint32_t usecHaD = testHaD();
  Serial.print(F("HaD pushColor            "));
  Serial.println(usecHaD);
  delay(300);

  uint32_t usecFillScreen = testFillScreen();
  Serial.print(F("Screen fill              "));
  Serial.println(usecFillScreen);
  delay(300);

  uint32_t usecText = testText();
  Serial.print(F("Text                     "));
  Serial.println(usecText);
  delay(300);

  uint32_t usecPixels = testPixels();
  Serial.print(F("Pixels                   "));
  Serial.println(usecPixels);
  delay(300);

  uint32_t usecLines = testLines(TFT_BLUE);
  Serial.print(F("Lines                    "));
  Serial.println(usecLines);
  delay(300);

  uint32_t usecFastLines = testFastLines(TFT_RED, TFT_BLUE);
  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(usecFastLines);
  delay(300);

  uint32_t usecRects = testRects(TFT_GREEN);
  Serial.print(F("Rectangles (outline)     "));
  Serial.println(usecRects);
  delay(300);

  uint32_t usecFilledRects = testFilledRects(TFT_YELLOW, TFT_MAGENTA);
  Serial.print(F("Rectangles (filled)      "));
  Serial.println(usecFilledRects);
  delay(300);

  uint32_t usecFilledCircles = testFilledCircles(10, TFT_MAGENTA);
  Serial.print(F("Circles (filled)         "));
  Serial.println(usecFilledCircles);
  delay(300);

  uint32_t usecCircles = testCircles(10, TFT_WHITE);
  Serial.print(F("Circles (outline)        "));
  Serial.println(usecCircles);
  delay(300);

  uint32_t usecTriangles = testTriangles();
  Serial.print(F("Triangles (outline)      "));
  Serial.println(usecTriangles);
  delay(300);

  uint32_t usecFilledTrangles = testFilledTriangles();
  Serial.print(F("Triangles (filled)       "));
  Serial.println(usecFilledTrangles);
  delay(300);

  uint32_t usecRoundRects = testRoundRects();
  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(usecRoundRects);
  delay(300);

  uint32_t usedFilledRoundRects = testFilledRoundRects();
  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(usedFilledRoundRects);
  delay(300);

  Serial.println(F("Done!"));

  // Pretty on-screen summary (fits 240x240)
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 0);
  tft.println(F("TFT_eSPI"));

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 28);
  tft.println(F("Benchmark (usec):"));

  tft.setCursor(0, 42);  tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("HaD: "));        tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecHaD);
  tft.setCursor(0, 54);  tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Fill: "));       tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecFillScreen);
  tft.setCursor(0, 66);  tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Text: "));       tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecText);
  tft.setCursor(0, 78);  tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Pixels: "));     tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecPixels);
  tft.setCursor(0, 90);  tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Lines: "));      tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecLines);
  tft.setCursor(0, 102); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("HV Lines: "));   tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecFastLines);
  tft.setCursor(0, 114); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Rects: "));      tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecRects);
  tft.setCursor(0, 126); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("FillRects: "));  tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecFilledRects);
  tft.setCursor(0, 138); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Circles: "));    tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecCircles);
  tft.setCursor(0, 150); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("FillCirc: "));   tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecFilledCircles);
  tft.setCursor(0, 162); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("Tri: "));        tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecTriangles);
  tft.setCursor(0, 174); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("FillTri: "));    tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecFilledTrangles);
  tft.setCursor(0, 186); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("RndRect: "));    tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usecRoundRects);
  tft.setCursor(0, 198); tft.setTextColor(TFT_CYAN, TFT_BLACK);  tft.print(F("FillRR: "));     tft.setTextColor(TFT_YELLOW, TFT_BLACK); printnice(usedFilledRoundRects);

  tft.setCursor(0, 224);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print(F("Done. Repeat in 30s"));

  delay(30000);
}

void printnice(int32_t v)
{
  char str[32] = {0};
  sprintf(str, "%d", (int)v);
  for (char *p = (str + strlen(str)) - 3; p > str; p -= 3) {
    memmove(p + 1, p, strlen(p) + 1);
    *p = ',';
  }
  while (strlen(str) < 7) {
    memmove(str + 1, str, strlen(str) + 1);
    *str = ' ';
  }
  tft.println(str);
}

static inline uint32_t micros_start()
{
  uint8_t oms = millis();
  while ((uint8_t)millis() == oms) { /* wait */ }
  return micros();
}

uint32_t testHaD()
{
  static const uint8_t HaD_128x160[] PROGMEM =
  {
    0x85, 0x91, 0x09, 0x4b, 0x09, 0x24, 0x0a, 0x47, 0x09, 0x27, 0x0a, 0x44, 0x0a, 0x29, 0x0a, 0x42,
    0x0a, 0x2b, 0x0a, 0x41, 0x0a, 0x2c, 0x0a, 0x3e, 0x0b, 0x2f, 0x09, 0x3d, 0x09, 0x32, 0x08, 0x3c,
    0x09, 0x33, 0x09, 0x3b, 0x08, 0x33, 0x0a, 0x3a, 0x0a, 0x31, 0x0b, 0x3a, 0x0c, 0x1d, 0x01, 0x10,
    0x0d, 0x39, 0x0c, 0x1d, 0x01, 0x10, 0x0d, 0x39, 0x0d, 0x0f, 0x01, 0x0c, 0x03, 0x0d, 0x0e, 0x39,
    0x0e, 0x0c, 0x03, 0x0c, 0x04, 0x0b, 0x0f, 0x39, 0x0f, 0x0a, 0x04, 0x0c, 0x05, 0x09, 0x10, 0x39,
    0x10, 0x08, 0x05, 0x0c, 0x06, 0x07, 0x11, 0x39, 0x11, 0x06, 0x06, 0x0d, 0x07, 0x04, 0x13, 0x37,
    0x12, 0x05, 0x07, 0x0d, 0x08, 0x02, 0x15, 0x34, 0x15, 0x03, 0x08, 0x0d, 0x20, 0x32, 0x20, 0x0e,
    0x21, 0x31, 0x20, 0x0f, 0x21, 0x2e, 0x22, 0x10, 0x22, 0x2b, 0x22, 0x12, 0x22, 0x12, 0x05, 0x12,
    0x22, 0x14, 0x22, 0x0c, 0x0f, 0x0c, 0x22, 0x16, 0x22, 0x08, 0x15, 0x08, 0x22, 0x18, 0x22, 0x05,
    0x19, 0x05, 0x21, 0x1c, 0x1f, 0x04, 0x1c, 0x05, 0x1f, 0x1f, 0x1c, 0x04, 0x1e, 0x04, 0x1d, 0x2b,
    0x11, 0x04, 0x21, 0x03, 0x12, 0x36, 0x0f, 0x03, 0x24, 0x03, 0x10, 0x38, 0x0d, 0x03, 0x26, 0x03,
    0x0d, 0x3b, 0x0b, 0x03, 0x28, 0x03, 0x0b, 0x3d, 0x0a, 0x03, 0x29, 0x03, 0x09, 0x40, 0x07, 0x03,
    0x2b, 0x03, 0x07, 0x42, 0x05, 0x03, 0x2c, 0x04, 0x04, 0x45, 0x04, 0x03, 0x2d, 0x03, 0x04, 0x46,
    0x02, 0x03, 0x2e, 0x03, 0x03, 0x48, 0x01, 0x03, 0x2f, 0x03, 0x01, 0x4c, 0x31, 0x4e, 0x32, 0x4e,
    0x33, 0x4c, 0x34, 0x4c, 0x34, 0x4c, 0x35, 0x4b, 0x35, 0x4a, 0x0e, 0x03, 0x14, 0x04, 0x0d, 0x4a,
    0x0b, 0x09, 0x0e, 0x0a, 0x0a, 0x4a, 0x0a, 0x0b, 0x0c, 0x0c, 0x09, 0x4a, 0x09, 0x0d, 0x0a, 0x0e,
    0x09, 0x49, 0x08, 0x0f, 0x09, 0x0e, 0x09, 0x49, 0x08, 0x0f, 0x09, 0x0f, 0x08, 0x49, 0x08, 0x0f,
    0x09, 0x0f, 0x08, 0x49, 0x07, 0x0f, 0x0a, 0x0f, 0x08, 0x49, 0x07, 0x0f, 0x0b, 0x0e, 0x08, 0x49,
    0x07, 0x0d, 0x0e, 0x0d, 0x08, 0x49, 0x07, 0x0b, 0x13, 0x0a, 0x08, 0x49, 0x08, 0x07, 0x18, 0x08,
    0x08, 0x49, 0x08, 0x06, 0x1b, 0x06, 0x08, 0x49, 0x09, 0x04, 0x1c, 0x05, 0x08, 0x4a, 0x09, 0x04,
    0x1d, 0x04, 0x08, 0x4a, 0x0a, 0x03, 0x1d, 0x03, 0x09, 0x4b, 0x19, 0x02, 0x1a, 0x4b, 0x19, 0x03,
    0x19, 0x4b, 0x18, 0x04, 0x18, 0x4d, 0x17, 0x05, 0x17, 0x4a, 0x01, 0x02, 0x17, 0x05, 0x16, 0x4a,
    0x02, 0x02, 0x17, 0x05, 0x16, 0x02, 0x03, 0x44, 0x03, 0x03, 0x16, 0x02, 0x01, 0x02, 0x16, 0x02,
    0x03, 0x43, 0x05, 0x03, 0x15, 0x01, 0x03, 0x01, 0x15, 0x03, 0x04, 0x41, 0x06, 0x03, 0x15, 0x01,
    0x03, 0x01, 0x14, 0x03, 0x07, 0x3d, 0x09, 0x03, 0x2d, 0x03, 0x08, 0x3b, 0x0a, 0x04, 0x2b, 0x03,
    0x0a, 0x39, 0x0c, 0x03, 0x2a, 0x04, 0x0b, 0x37, 0x0e, 0x03, 0x28, 0x03, 0x0e, 0x2e, 0x04, 0x03,
    0x10, 0x03, 0x27, 0x03, 0x10, 0x03, 0x03, 0x24, 0x19, 0x03, 0x26, 0x03, 0x1a, 0x1e, 0x1d, 0x03,
    0x24, 0x03, 0x1e, 0x19, 0x20, 0x04, 0x21, 0x03, 0x20, 0x17, 0x22, 0x04, 0x1f, 0x03, 0x22, 0x15,
    0x22, 0x04, 0x21, 0x04, 0x21, 0x13, 0x22, 0x05, 0x15, 0x01, 0x0b, 0x05, 0x21, 0x12, 0x21, 0x06,
    0x15, 0x01, 0x0b, 0x06, 0x21, 0x10, 0x21, 0x07, 0x0a, 0x01, 0x0a, 0x01, 0x0b, 0x07, 0x21, 0x0e,
    0x20, 0x0a, 0x09, 0x02, 0x09, 0x02, 0x09, 0x09, 0x20, 0x0e, 0x08, 0x02, 0x15, 0x0b, 0x08, 0x03,
    0x08, 0x03, 0x08, 0x0b, 0x15, 0x03, 0x08, 0x0d, 0x07, 0x04, 0x13, 0x0d, 0x06, 0x05, 0x06, 0x06,
    0x05, 0x0d, 0x14, 0x04, 0x07, 0x0c, 0x07, 0x06, 0x11, 0x38, 0x12, 0x06, 0x06, 0x0c, 0x06, 0x08,
    0x10, 0x39, 0x10, 0x08, 0x05, 0x0c, 0x04, 0x0b, 0x0f, 0x39, 0x0f, 0x0a, 0x04, 0x0c, 0x03, 0x0d,
    0x0e, 0x39, 0x0e, 0x0c, 0x03, 0x0c, 0x02, 0x0e, 0x0e, 0x39, 0x0d, 0x0f, 0x01, 0x0c, 0x01, 0x10,
    0x0d, 0x39, 0x0d, 0x0f, 0x01, 0x1e, 0x0c, 0x39, 0x0c, 0x30, 0x0a, 0x3a, 0x0a, 0x33, 0x09, 0x3b,
    0x08, 0x34, 0x09, 0x3b, 0x09, 0x32, 0x09, 0x3c, 0x0a, 0x2f, 0x0a, 0x3e, 0x0a, 0x2d, 0x0b, 0x3f,
    0x0a, 0x2b, 0x0b, 0x41, 0x0a, 0x29, 0x0b, 0x43, 0x0a, 0x27, 0x0a, 0x46, 0x0a, 0x25, 0x0a, 0x49,
    0x09, 0x23, 0x08, 0x4e, 0x08, 0x96, 0x12
  };

  tft.fillScreen(TFT_BLACK);

  uint32_t start = micros_start();

  // Draw 16 shades
  for (int i = 0; i < 0x10; i++)
  {
    // Center the 128x160 graphic on a 240x240 screen
    tft.setAddrWindow(56, 40, 128, 160);

    uint16_t cnt = 0;
    uint16_t color = tft.color565((i << 4) | i, (i << 4) | i, (i << 4) | i);
    uint16_t curcolor = 0;

    const uint8_t *cmp = &HaD_128x160[0];

    tft.startWrite();
    while (cmp < &HaD_128x160[sizeof(HaD_128x160)]) {
      cnt = pgm_read_byte(cmp++);
      if (cnt & 0x80) cnt = ((cnt & 0x7f) << 8) | pgm_read_byte(cmp++);
      tft.pushColor(curcolor, cnt);
      curcolor ^= color;
    }
    tft.endWrite();
  }

  uint32_t t = micros() - start;

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 220);
  tft.println(F("http://hackaday.io/     Xark"));

  delay(1000);
  return t;
}

uint32_t testFillScreen()
{
  uint32_t start = micros_start();
  tft.fillScreen(TFT_WHITE);
  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_GREEN);
  tft.fillScreen(TFT_BLUE);
  tft.fillScreen(TFT_BLACK);
  return (micros() - start) / 5;
}

uint32_t testText()
{
  tft.fillScreen(TFT_BLACK);
  uint32_t start = micros_start();
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(1);
  tft.println(F("Hello World!"));
  tft.setTextSize(2);
  tft.setTextColor(tft.color565(0xff, 0x00, 0x00));
  tft.print(F("RED "));
  tft.setTextColor(tft.color565(0x00, 0xff, 0x00));
  tft.print(F("GREEN "));
  tft.setTextColor(tft.color565(0x00, 0x00, 0xff));
  tft.println(F("BLUE"));
  tft.setTextColor(TFT_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(TFT_RED); tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(5);
  tft.println(F("Groop"));
  tft.setTextSize(2);
  tft.println(F("I implore thee"));
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);
  tft.println(F("my foonting turlingdromes."));
  tft.println(F("And hooptiously drangle me"));
  tft.println(F("with crinkly bindlewurdles,"));
  tft.println(F("Or I will rend thee"));
  tft.println(F("in the gobberwarts"));
  tft.println(F("with my blurglecruncheon,"));
  tft.println(F("see if I don't!"));
  tft.println();
  tft.println();
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(5);
  tft.println(F("Woot!"));
  uint32_t t = micros() - start;
  delay(200);
  return t;
}

uint32_t testPixels()
{
  int32_t w = tft.width();
  int32_t h = tft.height();

  uint32_t start = micros_start();
  tft.startWrite();
  for (uint16_t y = 0; y < h; y++) {
    for (uint16_t x = 0; x < w; x++) {
      tft.drawPixel(x, y, tft.color565(x<<3, y<<3, x*y));
    }
  }
  tft.endWrite();
  return micros() - start;
}

uint32_t testLines(uint16_t color)
{
  uint32_t start, t;
  int32_t x1, y1, x2, y2;
  int32_t w = tft.width();
  int32_t h = tft.height();

  tft.fillScreen(TFT_BLACK);

  x1 = y1 = 0;
  y2 = h - 1;

  start = micros_start();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  t = micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;

  start = micros_start();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  t += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1 = 0;
  y1 = h - 1;
  y2 = 0;

  start = micros_start();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  t += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;

  start = micros_start();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  t += micros() - start;

  return t;
}

uint32_t testFastLines(uint16_t color1, uint16_t color2)
{
  uint32_t start;
  int32_t x, y;
  int32_t w = tft.width();
  int32_t h = tft.height();

  tft.fillScreen(TFT_BLACK);

  start = micros_start();

  for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

uint32_t testRects(uint16_t color)
{
  uint32_t start;
  int32_t n, i, i2;
  int32_t cx = tft.width() / 2;
  int32_t cy = tft.height() / 2;

  tft.fillScreen(TFT_BLACK);
  n = min(tft.width(), tft.height());
  start = micros_start();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }
  return micros() - start;
}

uint32_t testFilledRects(uint16_t color1, uint16_t color2)
{
  uint32_t start, t = 0;
  int32_t n, i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = min(tft.width(), tft.height());
  for (i = n; i > 0; i -= 6) {
    i2 = i / 2;

    start = micros_start();
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
    t += micros() - start;

    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }

  return t;
}

uint32_t testFilledCircles(uint8_t radius, uint16_t color)
{
  uint32_t start;
  int32_t x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(TFT_BLACK);

  start = micros_start();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
  return micros() - start;
}

uint32_t testCircles(uint8_t radius, uint16_t color)
{
  uint32_t start;
  int32_t x, y, r2 = radius * 2;
  int32_t w = tft.width() + radius;
  int32_t h = tft.height() + radius;

  start = micros_start();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
  return micros() - start;
}

uint32_t testTriangles()
{
  uint32_t start;
  int32_t n, i;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = min(cx, cy);

  start = micros_start();
  for (i = 0; i < n; i += 5) {
    tft.drawTriangle(
      cx,     cy - i,
      cx - i, cy + i,
      cx + i, cy + i,
      tft.color565(0, 0, i)
    );
  }
  return micros() - start;
}

uint32_t testFilledTriangles()
{
  uint32_t start, t = 0;
  int32_t i;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);

  for (i = min(cx, cy); i > 10; i -= 5) {
    start = micros_start();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, tft.color565(i, i, 0));
  }
  return t;
}

uint32_t testRoundRects()
{
  uint32_t start;
  int32_t w, i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  w = min(tft.width(), tft.height());

  start = micros_start();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    tft.drawRoundRect(cx - i2, cy - i2, i, i, i/8, tft.color565(i, 0, 0));
  }
  return micros() - start;
}

uint32_t testFilledRoundRects()
{
  uint32_t start;
  int32_t i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);

  start = micros_start();
  for (i = min(tft.width(), tft.height()); i > 20; i -= 6) {
    i2 = i / 2;
    tft.fillRoundRect(cx - i2, cy - i2, i, i, i/8, tft.color565(0, i, 0));
  }
  return micros() - start;
}
