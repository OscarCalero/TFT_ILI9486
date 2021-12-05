
// Oscar Calero demo code for TFT ILI9486 with Touchpad
// Based on Bodmer github repository examples for TFTs with my thanks.

// A copy of JPEG Decoder library can be downloaded here, it is based on the library by Makoto Kurauchi.
// https://github.com/Bodmer/JPEGDecoder
// confiuration settings on file: User_Setup.h --> ...\Documents\Arduino\libraries\TFT_eSPI-master

// TFT_eSPI library
// https://github.com/Bodmer/TFT_eSPI

// Images on SD Card must be put in the root folder (top level) to be found
// /parquec.jpg
// /parqueg.jpg
// /casac.jpg
// /casag.jpg
//
// For touch calibration file is required SPIFFS partition scheme
//----------------------------------------------------------------------------------------------------

// ********** HARDWARE for TFT ILI9486 480x320**********
//SPI port for ESP32 && TFT ILI9486 480x320 with touch & SD card reader
//devices MOSI --> ESP32 PIN 23
//devices MISO --> ESP32 PIN 19
//devices clk --> ESP32 PIN 18
//TFT RST --> PIN 4
//TFT D/C --> PIN 2
//TFT BL --> PIN 3v3 (LED)
//TFT CS to PIN 15 (confirm in User_Setup.h)
//SD CS to PIN 5 (but you can change)
//Touch CS to PIN 21 (confirm in User_Setup.h)

// ********** General **********
#include <SPI.h>
#include <FS.h>

// ********** SD card **********
#define SD_CS 5  //SD CS to PIN 5 --> if different should specifed in SD.begin(PIN)
#include <SD.h>
bool existSD = true;


// ********** TFT_eSPI screen **********
#define TFT_CS 15  //TFT CS to PIN 15
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <JPEGDecoder.h> // ********** JPEG decoder  ********** in case JPG images used


// ********** TFT_eSPI touch **********
#define TOUCH_CS 21  //Touch CS to PIN 21
#define CALIBRATION_FILE "/TouchCalData2" // Calibration file stored in SPIFFS
#define REPEAT_CAL false // if true calibration is requested after reboot
#define totalButtonNumber 2
bool casa;
bool parque;


#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2
TFT_eSPI_Button key[totalButtonNumber];  // TFT_eSPI button class


//####################################################################################################
// Setup
//####################################################################################################
void setup() {

  // Set all chip selects high to avoid bus contention during initialisation of each peripheral
  digitalWrite(TOUCH_CS, HIGH); // ********** TFT_eSPI touch **********
  digitalWrite(TFT_CS, HIGH); // ********** TFT_eSPI screen library **********
  digitalWrite(SD_CS, HIGH); // ********** SD card **********

  Serial.begin(115200);
  delay (2000);

  // ********** SD card **********
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    existSD = false;
    //return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    existSD = false;
    //return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);


  // ********** TFT_eSPI screen library **********
  tft.begin();
  tft.invertDisplay(false); // I don't know why but it is required for my TFT to color correction
  tft.setRotation(1);  // Landscape
  touch_calibrate();  // Calibrate the touch screen and retrieve the scaling factors recorded in SD/SPIFFS


  // ********** General **********
  Serial.println("initialisation done...");


  // ********** First print **********
  int defcolor = ConvertRGBto565(131, 131, 131);
  buttons();
  casac(true);
  parquec(true);
}


//####################################################################################################
// Main loop
//####################################################################################################
void loop() {

  uint16_t t_x = 0, t_y = 0; // coordenadas pulsacion
  bool pressed = tft.getTouch(&t_x, &t_y);  // true al pulsar

  // Comprueba si pulsas en zona de botón
  for (uint8_t b = 0; b < totalButtonNumber; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);
      Serial.print(t_x);
      Serial.print(",");
      Serial.println(t_y);
    } else {
      key[b].press(false);
    }
  }

  // Accion si se pulsa boton
  for (uint8_t b = 0; b < totalButtonNumber; b++) {

    if (key[b].justReleased()) {
      key[b].drawButton(); // redibuja al soltar

      switch (b) {
        case 0:
          status("system Enabled");
          casac(true);
          parquec(true);
          break;
        case 1:
          status("system Disabled");
          casac(false);
          parquec(false);
          break;
        default:
          delay(1);
          // statements
      }
    }
    if (key[b].justPressed()) {
      key[b].drawButton(true);  // cambia color del botón al pulsar
      delay(10); // UI debouncing
    }
  }

  // Comprueba si pulsas en imágenes
  if (pressed) {
    Serial.println("pulsado sobre imagen");

    if ( t_x > 200 && t_x < 200 + 267 && t_y > tft.height() - 270 && t_y < tft.height() ) { // sobre casa
      if (casa) {
        casac(false);
      } else {
        casac(true);
      }
    }
    if ( t_x > 10 && t_x < 10 + 187 && t_y > tft.height() - 147 && t_y < tft.height() ) { // sobre parque
      if (parque) {
        parquec(false);
      } else {
        parquec(true);
      }
    }

    // publica texto de estatus en la pantalla
    if (casa && parque) {
      status("system Enabled");
    }
    else if (!casa && !parque) {
      status("system Disabled");
    }
    else if (casa ) {
      status("house Enabled");
    }
    else  {
      status("parc Enabled");
    }

    delay(10); // evitar rebotes de pulsacion
  }

}


//####################################################################################################
// Dibuja CASA
//####################################################################################################
void casac(bool act) {
  int x;
  int y;
  x = 200;
  y = tft.height() - 270;
  Serial.println("dibuja casa");
  if (act) {
    casa = true;
    drawSdJpeg("/casac.jpg", x, y);
  } else {
    casa = false;
    drawSdJpeg("/casag.jpg", x, y);
  }
}

//####################################################################################################
// Dibuja PARQUE
//####################################################################################################
void parquec(bool act) {
  int x;
  int y;
  x = 10;
  y = tft.height() - 147 - 1;
  Serial.println("dibuja parque");
  if (act) {
    parque = true;
    drawSdJpeg("/parquec.jpg", x, y);
  } else {
    parque = false;
    drawSdJpeg("/parqueg.jpg", x, y);
  }

}


//####################################################################################################
// Draw a JPEG on the TFT pulled from SD Card
//####################################################################################################
// xpos, ypos is top left corner of plotted image
void drawSdJpeg(const char *filename, int xpos, int ypos) {

  // Open the named file (the Jpeg decoder library will close it)
  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library

  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  Serial.println("===========================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("===========================");

  // Use one of the following methods to initialise the decoder:
  bool decoded = JpegDec.decodeSdFile(jpegFile);  // Pass the SD file handle to the decoder,
  //bool decoded = JpegDec.decodeSdFile(filename);  // or pass the filename (String or character array)

  if (decoded) {
    // print information about the image to the serial port
    //jpegInfo();
    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos) {

  //jpegInfo(); // Print information from the JPEG file (could comment this line out)

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
  uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ( (mcu_y + win_h) >= tft.height())
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }

  tft.setSwapBytes(swapBytes);

}


//####################################################################################################
// RGB 24 bits to RGB565 (16bits) conversion
//####################################################################################################
int ConvertRGBto565(byte rr, byte gg, byte bb)
{
  //reduz para 5 bits significativos
  byte r = (byte) (rr >> 3);
  //reduz para 6 bits significativos
  byte g = (byte)(gg >> 2);
  //reduz para 5 bits significativos
  byte b = (byte)(bb >> 3);

  //Junta
  return (int)((r << 11) | (g << 5) | b);
}

//####################################################################################################
// screen calibration ********** TFT_eSPI touch **********
//####################################################################################################
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (existSD) {
    // check if calibration file exists and size is correct
    if (SD.exists(CALIBRATION_FILE)) {
      if (REPEAT_CAL)
      {
        // Delete if we want to re-calibrate
        SD.remove(CALIBRATION_FILE);
      }
      else
      {
        File f = SD.open(CALIBRATION_FILE, "r");
        if (f) {
          if (f.readBytes((char *)calData, 14) == 14)
            calDataOK = 1;
          f.close();
        }
      }
    }
  }
  else  // SPIFFS uses
  {
    // check file system exists
    if (!SPIFFS.begin()) {
      Serial.println("Formating file system");
      SPIFFS.format();
      SPIFFS.begin();
    }

    // check if calibration file exists and size is correct
    if (SPIFFS.exists(CALIBRATION_FILE)) {
      if (REPEAT_CAL)
      {
        // Delete if we want to re-calibrate
        SPIFFS.remove(CALIBRATION_FILE);
      }
      else
      {
        File f = SPIFFS.open(CALIBRATION_FILE, "r");
        if (f) {
          if (f.readBytes((char *)calData, 14) == 14)
            calDataOK = 1;
          f.close();
        }
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    Serial.print(calData[1]);
    Serial.print(",");
    Serial.print(calData[2]);
    Serial.print(",  ");
    Serial.print(calData[3]);
    Serial.print(",");
    Serial.print(calData[4]);
    Serial.print(",  ");
    Serial.print(calData[5]);
    Serial.print(",");
    Serial.print(calData[6]);
    Serial.print(",  ");
    Serial.print(calData[7]);
    Serial.print(",");
    Serial.println(calData[8]);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    if (existSD) {
      File f = SD.open(CALIBRATION_FILE, "w");
      if (f) {
        f.write((const unsigned char *)calData, 14);
        f.close();
      }
    }
    else {
      File f = SPIFFS.open(CALIBRATION_FILE, "w");
      if (f) {
        f.write((const unsigned char *)calData, 14);
        f.close();
      }
    }

  }
}


//####################################################################################################
// button ********** TFT_eSPI touch **********
//####################################################################################################
void buttons()
{
  int defcolor = ConvertRGBto565(131, 131, 131);
  tft.setRotation(1);
  tft.fillScreen(defcolor);
  // Draw the keys
  tft.setFreeFont(LABEL1_FONT);
  char keyLabel[totalButtonNumber][8] = {"Enable", "Disable" };
  key[0].initButton(&tft, 80, 40, 110, 60, TFT_BLACK, TFT_WHITE, TFT_BLUE, keyLabel[0] , 1 ); // x, y, w, h, outline, fill, color, label, text_Size
  key[0].drawButton();
  key[1].initButton(&tft, 80, 115, 110, 60, TFT_BLACK, TFT_WHITE, TFT_BLUE, keyLabel[1] , 1 );
  key[1].drawButton();

}

//####################################################################################################
// Print status text
//####################################################################################################
void status(const char *msg) {
  int defcolor = ConvertRGBto565(131, 131, 131);
  tft.setTextPadding(200);
  tft.setTextColor(TFT_BLACK, defcolor);
  tft.drawString(msg, 200, 20);
}
