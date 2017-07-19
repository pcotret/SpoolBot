// Libraries
#include <LiquidCrystal.h>
#include <ps2.h>

// Initialize the LCD screen
LiquidCrystal lcd(12,11,5,4,3,2);

// Connect the PS/2 mouse: data on pin 6, clock on pin 7
PS2 mouse(7,6);

// Initialize the mouse structure
void mouse_init()
{
  mouse.write(0xff); // Reset byte
  mouse.read();      // Ack byte
  mouse.read();      // Blank byte
  mouse.read();      // Blank byte
  mouse.write(0xf0); // Remote mode byte
  mouse.read();      // Ack byte
}

// Global variables: the mouse position
float x_position=0;
float total_position=0;
float total_length=0;
float current_length=0;
char mouse_state;
char mouse_x;
char mouse_y;

// Setup (executed only once)
void setup()
{
  Serial.begin(9600);// Initialize the serial link	
  mouse_init();
  lcd.begin(16,2); // Set LCD numbers of lines/rows
}

// Main loop (infinite)
void loop()
{
  // Set the LCD position to [0,0]
  lcd.setCursor(0,0);
  // Print the total length of filament used
  lcd.print("Total: ");
  lcd.print(total_length);
  lcd.setCursor(0,1);
  // Print the current length of filament used
  lcd.print("Current: ");
  lcd.print(current_length);
  // Transmit data with the mouse
  mouse.write(0xeb);        // Asking the mouse to send us data
  mouse.read();             // Ignore the acknowledgment bit
  mouse_state=mouse.read(); // Read mouse state
  mouse_x = mouse.read();   // Get X movement
  mouse_y = mouse.read();   // Get Y movement
  // Computing the absolute position
  x_position=x_position+mouse_x;
  current_length=x_position*0.038;
  total_position=total_position+mouse_x;
  total_length=total_position*0.038;
  // Displaying results in the serial monitor
  Serial.print("Current: ");
  Serial.print(current_length);
  Serial.print("Total: ");
  Serial.print(total_length);
  // If left button is pressed, reset the total length
  if(mouse_state&(1<<0))
  {
    Serial.print("Resetting total counter!");
    total_length=0;
    total_position=0;
    lcd.clear();
  }
  // If right button is pressed, reset the current length
  if(mouse_state&(1<<1))
  {
    Serial.print("Resetting current counter!");
    current_length=0;
    x_position=0;
    lcd.clear();
  }
  Serial.println();
}