//************************************************
//*                learnelectronics              *
//*                Arduino Ohm Meter             *
//*     Original code by Iasonas Christoulakis   *
//*     Modified by learnelectronics (9/28/21)   *
//*     Additional comments & changes noted      *
//*        www.youtube.com/learnelectronics      *
//*                                              *
//*           email: arduino0169@gmail.com       *
//************************************************

//****************** CHANGES ********************
// 1. Switched display from LCD to OLED (I2C-based).
// 2. Adjusted Vin from 5V to 4.7V for accuracy.
// 3. Changed reference resistor (R1) from 1kΩ to 4.651kΩ.
// 4. Updated display functions for Adafruit SSD1306 OLED.
// 5. Reduced delay from 1s to 100ms for faster updates.
//***********************************************

#include <Wire.h>                      // Library for I2C communication
#include <Adafruit_SSD1306.h>          // OLED display driver

#define OLED_RESET 4                   // Required for OLED display
Adafruit_SSD1306 display(OLED_RESET);  // Initialize OLED display

int analogPin = 0;                     // Analog pin for voltage divider
int raw = 0;                            // Raw ADC reading
float Vin = 4.7;                        // Adjusted voltage from Arduino (previously 5V)
float Vout = 0;                         // Measured output voltage
float R1 = 4651;                        // Updated known resistor value (was 1000Ω)
float R2 = 0;                           // Unknown resistor (to be calculated)
float buffer = 0;                       // Temporary calculation buffer

void setup() {                
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize OLED display
    display.display();                           // Refresh display
    delay(2);                                    // Short delay
    display.clearDisplay();                      // Clear the screen
}

void loop() {
    raw = analogRead(analogPin);                // Read voltage from analog pin
    if (raw) {                                  // Ensure valid reading
        buffer = raw * Vin;                     // Multiply ADC value by Vin
        Vout = buffer / 1024.0;                 // Convert to voltage
        buffer = (Vin / Vout) - 1;              // Calculate ratio
        R2 = R1 * buffer;                       // Compute unknown resistance
    }

    //********** Display Results on OLED **********
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.print("Vout: ");
    display.println(Vout, 3);  // Print voltage with 3 decimal places
    display.print("R2: ");
    display.println(R2, 1);    // Print resistance with 1 decimal place

    display.display();         // Refresh the screen
    
    delay(100);                // Reduced delay (was 1000ms)
}
