/*
  This is code for a monster-in-a-box halloween project.
  A PIR motion sensor triggers several 12V linear actuators
  arranged inside a box so as to shake the box - simulating
  a monster trying to get out.  In addition, the same
  motion sensor triggers monster growling sounds to be
  played over speakers mounted inside the box as well.
*/

// Pin definitions.
int led_pin = 13;        // LED on Pin 13 of Arduino
int pir_pin = 7;         // Input for HC-SR501 PIR motion sensor
int pir_value;           // Place to store read PIR Value
int audio_pin = 2;       // Output for relay 1
int shaker_lid_pin = 4;  // Output for relay 2
int left_eye_pin = 8;    // Output for relay 3
int right_eye_pin = 12;   // Output for relay 4

// State machine modes.
#define SENSE_MODE 0
#define BEAST_MODE 1
#define REST_MODE 2

// Other helpful variables.
unsigned int mode = SENSE_MODE;
unsigned int counter = 0;
unsigned int beast_mode_timeout = 2600;
unsigned int rest_mode_timeout = 1000;

// Modulus values for timers.
unsigned int mod_lid = 47;
unsigned int duration_lid = 8;
unsigned int mod_left_eye = 650;
unsigned int mod_right_eye = 650;
unsigned int duration_blink = 64;

// Setup the pins as inputs/outputs, etc. and write their initial state.
void setup() {

  Serial.begin(9600);

  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  pinMode(pir_pin, INPUT);

  pinMode(audio_pin, OUTPUT);
  digitalWrite(audio_pin, LOW);

  pinMode(shaker_lid_pin, OUTPUT);
  digitalWrite(shaker_lid_pin, LOW);

  pinMode(left_eye_pin, OUTPUT);
  digitalWrite(left_eye_pin, LOW);

  pinMode(right_eye_pin, OUTPUT);
  digitalWrite(right_eye_pin, LOW);

  // The cheap PIR sensor needs a full minute for proper boot-up.
  // Flash the led_pin while we wait.
  for (int ii = 0; ii < 120; ++ii) {
    digitalWrite(led_pin, HIGH);
    delay(350);
    digitalWrite(led_pin, LOW);
    delay(150);
  }
}

// The main loop.
void loop() {
  // Print the Mode.
  Serial.println(mode);

  // Increment the counter.
  counter++;

  // Read the PIR sensor and keep the LED sync'd.
  pir_value = digitalRead(pir_pin);
  digitalWrite(led_pin, pir_value);

  // Handle the system modes.
  switch (mode) {

    case SENSE_MODE:
      if (pir_value == HIGH) {
        // Turn on the audio.
        digitalWrite(audio_pin, HIGH);

        // Switch to beast mode.
        mode = BEAST_MODE;
        counter = 0;
      }
      break;

    case BEAST_MODE:
      // Set the shakers according to their modulus values.
      // Higher modulus = lower frequency.
      if (counter % mod_left_eye == 0 ) {
        digitalWrite(left_eye_pin, LOW);
      } else if (counter % mod_left_eye == duration_blink ) {
        digitalWrite(left_eye_pin, HIGH);
      }
      if (counter % mod_right_eye == 0 ) {
        digitalWrite(right_eye_pin, LOW);
      } else if (counter % mod_right_eye == duration_blink ) {
        digitalWrite(right_eye_pin, HIGH);
      }
      if (counter % mod_lid == 0 ) {
        digitalWrite(shaker_lid_pin, HIGH);
      } else if (counter % mod_lid == duration_lid ) {
        digitalWrite(shaker_lid_pin, LOW);
      }

      // If the Beast mode counter is expired, move to Rest mode.
      if (counter >= beast_mode_timeout) {
        // Reset the audio and the shakers.
        digitalWrite(audio_pin, LOW);
        digitalWrite(shaker_lid_pin, LOW);
        digitalWrite(left_eye_pin, LOW);
        digitalWrite(right_eye_pin, LOW);
        mode = REST_MODE;
        counter = 0;
      }
      break;

    case REST_MODE:
      // If the counter is expired, reset to Sense mode.
      if (counter >= rest_mode_timeout) {
        mode = SENSE_MODE;
      }
      break;
  } // switch

} // loop
