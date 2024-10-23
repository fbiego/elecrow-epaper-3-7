#include <Arduino.h>            // Include the Arduino core library
#include "EPD.h"                // Include the e-paper display library
#include "EPD_GUI.h"            // Include the e-paper display GUI library
#include "Ap_29demo.h"          // Custom library file

#include "lvgl.h"


#define WIDTH 416
#define HEIGHT 240
#define BUF_SIZE 50
#define LVBUF (WIDTH * BUF_SIZE) // ((WIDTH * HEIGHT / 8) + 8 )

static lv_display_t *lvDisplay;
static uint8_t lvBuffer[2][LVBUF];

// Define the size of the e-paper image buffer
uint8_t ImageBW[12480]; // E-paper image buffer


void my_disp_flush(lv_display_t *disp, const lv_area_t *area, unsigned char *data)
{

    for (int y = area->y1; y <= area->y2; y++)
    {
        for (int x = area->x1; x <= area->x2; x++)
        {
            // Get the grayscale value from LVGL's buffer (8-bit grayscale)
            uint8_t grayscale = *data;
            data++;

            // Convert grayscale to 1-bit (black or white) based on threshold
            Paint_SetPixel(x, y, grayscale > 128);
        }
    }

    EPD_Display(ImageBW);

    EPD_Update();

    // Inform LVGL that flushing is complete
    lv_display_flush_ready(disp);
}


static uint32_t my_tick(void)
{
    return millis();
}


void setup() {
  Serial.begin(115200);  // Start serial communication, set baud rate to 115200
  
  // Set the screen power pin and start the screen
  pinMode(7, OUTPUT);  // Set pin 7 to output mode
  digitalWrite(7, HIGH);  // Power the screen

  EPD_GPIOInit();  // Initialize the screen GPIO
  Paint_NewImage(ImageBW, EPD_W, EPD_H, 180, WHITE);  // Create a new image canvas
  Paint_Clear(WHITE);  // Clear the canvas


  EPD_FastInit();  // Fast initialize the screen
  EPD_Display_Clear();  // Clear the screen display


  EPD_DrawRectangle(10, 20, 100, 150, BLACK, false);
  EPD_Display(ImageBW); // Update the screen content
  EPD_Update(); // Refresh the screen content


  lv_init();

  lv_tick_set_cb(my_tick);

  lvDisplay = lv_display_create(WIDTH, HEIGHT);
  lv_display_set_flush_cb(lvDisplay, my_disp_flush);
  lv_display_set_buffers(lvDisplay, lvBuffer[0], lvBuffer[1], LVBUF, LV_DISPLAY_RENDER_MODE_PARTIAL);


  lv_obj_t *label1 = lv_label_create(lv_scr_act());
  lv_obj_set_align(label1, LV_ALIGN_CENTER);
  lv_obj_set_width(label1, 300);
  lv_label_set_text(label1, "Hello from Elecrow ePaper ESP32 S3, LVGL 9 here!");
  lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void loop() {
  lv_timer_handler(); // Update the UI-
  delay(1000);
}