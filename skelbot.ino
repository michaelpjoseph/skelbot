/*  2017 halloween animitronic skull project
 *    October 9, 2017
 *    -MPJ- Michael P Joseph - michaelpjoseph@gmail.com
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();   // pwm object to drive 9685 board - use default addr 0x40

/*  ---- ACTUAL Y AXIS FORWARD AND BACKWARD RANGE OF MOTION WITH ONLY U BRACKET
#define Y_SERVOMIN  180 // head back
#define Y_SERVOMAX  512 // head forward
*/

/* ---- LIMITED Y AXIS HEAD MOTION TO PREVENT SKULL COLLISION WITH BOX ---- */
#define Y_SERVOMIN  220 // head back   ---  maps to approx 35 degrees
#define Y_SERVOMAX  380 // head forward  --- maps to approx 105 degrees
#define Y_SERVO90   345

#define X_SERVOMIN  150 // look left (from my perspective)
#define X_SERVOMAX  535 // look right

/* ---- SET UP NEOPIXEL EYES ---- */
#define NUMPIX 7  // pixels on the NeoPixel ring
#define L_EYE_PIN 13
#define R_EYE_PIN 12

Adafruit_NeoPixel l_eye = Adafruit_NeoPixel(NUMPIX, L_EYE_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel r_eye = Adafruit_NeoPixel(NUMPIX, R_EYE_PIN, NEO_GRB + NEO_KHZ800);

int x_srv_pos = 0,
    y_srv_pos = 300;

//define servo position on i2c 9685 PWM breakout board
uint8_t y_servonum = 15;
uint8_t x_servonum = 14;

void setup() {
  Serial.begin(9600);

  // -- start i2c pwm object
  pwm.begin();
  pwm.setPWMFreq(60);  // standard 60 Hz servo refresh
  yield();  // I have no idea what this is used for... it was part of example sketch.

  x_srv_goto(90);
  y_srv_goto(90, 10);
  delay(2000);

  l_eye.begin();
  r_eye.begin();
  //y_srv_sweepstart();  // no longer needed.  see function for original purpose.
}


void loop() {
  eyes_lookforward_red(l_eye);
  
  
  lookrandom();
  
  y_srv_goto(90, 10);
  delay(2000);
  x_srv_goto_slow(45, 20);
  delay(1000);
  x_srv_goto_slow(115, 10);
  delay(1000);
  x_srv_goto_slow(90, 10);
  delay(3000);
  x_srv_goto(125);
  eyes_lightningcrash(l_eye);
  eyes_lookforward_red(l_eye);
  delay(2000);
  x_srv_goto_slow(90, 10);
  delay(1000);
  y_srv_goto(115, 10);
  delay(1000);
}

void lookrandom() {
  int x_dir = 0;
  int y_dir = 105;
  bool alt_eyes = true;
  for (int i = 0; i <= 20; i++) {
    if (alt_eyes) {
      eyes_lookforward_red(l_eye);
    } else {
      eyes_lookforward(l_eye);
    }
    x_dir = rand() % 180;
    int y_dir = rand() % 35 + 70;
    //x_srv_goto(x_dir);
    //y_srv_goto(y_dir,10);
    head_goto_xy(x_dir, y_dir, 3);
    alt_eyes = !alt_eyes;
  }
}



void y_srv_goto(int angle, int del) {
  int angle_pulselen = y_srv_map_angle(angle);
  Serial.println(" -=  entering y servo goto =-");
  Serial.print("y on entrance:  ");  Serial.println(y_srv_pos);
  Serial.print("req angle: "); Serial.print(angle); Serial.print(" - "); Serial.println(angle_pulselen);
  // -- sanity check on angle
  if (angle_pulselen > Y_SERVOMAX ) {
    Serial.println(" !!! REQUESTED ANGLE TOO LARGE");
    angle_pulselen = Y_SERVOMAX;
  } else if (angle_pulselen < Y_SERVOMIN) {
    Serial.println(" !!! REQUESTED ANGLE TOO SMALL");
    angle_pulselen = Y_SERVOMIN;
  }
  if (y_srv_pos > angle_pulselen) {
    for (y_srv_pos; y_srv_pos >= angle_pulselen; y_srv_pos--) {
      Serial.println(y_srv_pos);
      pwm.setPWM(y_servonum, 0, y_srv_pos);
      delay(del);
    }
  } else if (y_srv_pos < angle_pulselen) {
    for (y_srv_pos; y_srv_pos <= angle_pulselen; y_srv_pos++) {
      Serial.println(y_srv_pos);
      pwm.setPWM(y_servonum, 0, y_srv_pos);
      delay(del);
    }
  }
}

void x_srv_goto_slow(int angle, int del) {
  int angle_pulselen = x_srv_map_angle(angle);
  Serial.println(" -=  entering x servo goto =-");
  Serial.print("x on entrance:  ");  Serial.println(x_srv_pos);
  Serial.print("req angle: "); Serial.print(angle); Serial.print(" - "); Serial.println(angle_pulselen);
  // -- sanity check on angle
  if (angle_pulselen > X_SERVOMAX ) {
    Serial.println(" !!! REQUESTED ANGLE TOO LARGE");
    angle_pulselen = X_SERVOMAX;
  } else if (angle_pulselen < X_SERVOMIN) {
    Serial.println(" !!! REQUESTED ANGLE TOO SMALL");
    angle_pulselen = X_SERVOMIN;
  }
  if (x_srv_pos > angle_pulselen) {
    for (x_srv_pos; x_srv_pos >= angle_pulselen; x_srv_pos--) {
      Serial.println(x_srv_pos);
      pwm.setPWM(x_servonum, 0, x_srv_pos);
      delay(del);
    }
  } else if (x_srv_pos < angle_pulselen) {
    for (x_srv_pos; x_srv_pos <= angle_pulselen; x_srv_pos++) {
      Serial.println(x_srv_pos);
      pwm.setPWM(x_servonum, 0, x_srv_pos);
      delay(del);
    }
  }
}

int x_srv_map_angle(int angle) {
  return map(angle, 0, 180, X_SERVOMIN, X_SERVOMAX);
}

int y_srv_map_angle(int angle) {
  return map(angle, 35, 105, Y_SERVOMIN, Y_SERVOMAX);  // head can only move between 35 and 105 degrees to avoid collision with box
}

int y_srv_map_angle_reverse(int pulselen) {
  return map(pulselen, Y_SERVOMIN, Y_SERVOMAX, 35, 105);
}

bool x_srv_isSane(int angle_pulselen) {
  if (angle_pulselen > X_SERVOMAX ) {
    Serial.println(" !!! REQUESTED ANGLE TOO LARGE");
    return false;
  } else if (angle_pulselen < X_SERVOMIN) {
    Serial.println(" !!! REQUESTED ANGLE TOO SMALL");
    return false;
  } else {
    return true;
  }
}

bool y_srv_isSane(int angle_pulselen) {
  if (angle_pulselen > Y_SERVOMAX ) {
    Serial.println(" !!! REQUESTED ANGLE TOO LARGE");
    return false;
  } else if (angle_pulselen < Y_SERVOMIN) {
    Serial.println(" !!! REQUESTED ANGLE TOO SMALL");
    return false;
  } else {
    return true;
  }
}


void x_srv_goto(int angle) {
  // -- sanity check on angle
  int angle_pulselen = x_srv_map_angle(angle);
  if (x_srv_isSane(angle_pulselen)) {
    pwm.setPWM(x_servonum, 0, x_srv_map_angle(angle));
    x_srv_pos = x_srv_map_angle(angle);
  }
}

void head_goto_xy(int x_angle, int y_angle, int del) {
  Serial.print("(head_goto_xy) - entering - x: "); Serial.print(x_angle); Serial.print("\ty:"); Serial.println(y_angle);
  int x_angle_pulselen = x_srv_map_angle(x_angle);  // convert 0-180 degree to associated pulse lengths
  int y_angle_pulselen = y_srv_map_angle(y_angle);
  int x_diff = abs(x_srv_pos - x_angle_pulselen);  // find difference between current location and new locations
  int y_diff = abs(y_srv_pos - y_angle_pulselen);
  int largest_diff = 0;  // holds the largest diff between current pos and new angle
  int x_dir = 0, y_dir = 0;  // directional variables for head movement
  
  // find largest difference between current head position and provided coordinates
  if (x_diff > y_diff) { 
    largest_diff = x_diff;
  } else if (y_diff > x_diff) {
    largest_diff = y_diff;
  } else {
    Serial.println("(head_goto_xy) - no change between head position and provided x, y coordinates");
    return;  // exit function
  }
  
  // find direction to move along both x and y axis based on current head position and provided coordnates
  if (x_angle_pulselen > x_srv_pos) {
    x_dir = 1;
  } else if (x_angle_pulselen < x_srv_pos) {
    x_dir = -1;
  }
  if (y_angle_pulselen > y_srv_pos) {
    y_dir = 1;
  } else if (y_angle_pulselen < y_srv_pos) {
    y_dir = -1;
  }
/*  ******** DEBUG OUTPUT
  Serial.println(" ... ");
  Serial.print("++ X Strt: "); Serial.println(x_srv_pos);
  Serial.print("++ X New: "); Serial.println(x_angle_pulselen);
  Serial.print("++ X Diff: "); Serial.println(x_diff);
  Serial.print("++ X Dir: "); Serial.println(x_dir);
  
  Serial.print("++ Y Strt: "); Serial.println(y_srv_pos);
  Serial.print("++ Y New: "); Serial.println(y_angle_pulselen);
  Serial.print("++ Y Diff: "); Serial.println(y_diff);
  Serial.print("++ Y Dir: "); Serial.println(y_dir);
  Serial.print("++ Diff: "); Serial.println(largest_diff);
  delay(5000);
*   ********/
  // move to position
  int i = 0;
  while (i <= largest_diff) {
    if (x_srv_pos != x_angle_pulselen) {
      //if (x_srv_isSane((x_srv_pos + x_dir))) {
        x_srv_pos += x_dir;
        pwm.setPWM(x_servonum, 0, x_srv_pos);
        //Serial.print("X - "); Serial.println(x_srv_pos);
        //delay(del);
      //}
    }
    if (y_srv_pos != y_angle_pulselen) {
      //Serial.println(" .");
      //if (y_srv_isSane((y_srv_pos + y_dir))) {
        y_srv_pos += y_dir;
        pwm.setPWM(y_servonum, 0, y_srv_pos);
        //Serial.print("Y - "); Serial.println(y_srv_pos);
        //delay(del);
      //}
    }
    i++;
    delay(del);
    //Serial.print("----------- "); Serial.print(i); Serial.print(" / "); Serial.println(largest_diff);
    //delay(del);
  }
}

// ---  eye routines
void cleareyes(Adafruit_NeoPixel eye) {
  eye.setBrightness(0);
  eye.show();
  eye.setBrightness(255);
}

void eyes_lightningcrash(Adafruit_NeoPixel eye) {
  for (int i = 0; i <= eye.numPixels(); i++) {
    eye.setPixelColor(i    , 0, 0, 255);
  }
  eye.show();
  delay(15);
  eye.setBrightness(0);
  eye.show();
  eye.setBrightness(255);
  delay(250);
  //-- inital blast end
  //-- rapid blast start
  for (int times = 0; times < 4; times++) {  
    for (int i = 0; i <= eye.numPixels(); i++) {
      eye.setPixelColor(i    , 255, 255, 255);
    }
    eye.show();
    delay(50);
    eye.setBrightness(0);
    eye.show();
    eye.setBrightness(255);
    delay(50);
  }
  //-- rapid blast end
  //-- random color and decay sequence
  for (int i = 0; i <= eye.numPixels(); i++) {  
    int red=rand() % 255;
    int gre=rand() % 255;
    int blu=rand() % 255;
    eye.setPixelColor(i, eye.Color(red, gre, blu));
  }
  eye.show();
  for (int i = 255; i >= 0; i-- ) {
    eye.setBrightness(i);
    eye.show();
    delay(2);
  }
  delay(10);
  eye.setBrightness(255);
}

void eyes_circlefade(Adafruit_NeoPixel eye) {
  int rndpix = 1;
  int rndbrt = 0;
  for (int i = 1; i <= eye.numPixels(); i++) {
    eye.setPixelColor(0, 255, 0, 0);
    rndpix = rand() % 6 + 1;
    rndbrt = rand() % 200 + 1;
    eye.setPixelColor(rndpix    , rndbrt, 25, 25);
    eye.show();
    delay(10);
    cleareyes();
    delay(10);
  }
}

void eyes_topdown(Adafruit_NeoPixel eye) {
  int del=50;
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(1    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(2    , 255, 0, 0);
  eye.setPixelColor(6    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(3    , 255, 0, 0);
  eye.setPixelColor(5    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(4    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.show();
}

void eyes_bottomup(Adafruit_NeoPixel eye) {
  int del = 50;
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(4    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(3    , 255, 0, 0);
  eye.setPixelColor(5    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(2    , 255, 0, 0);
  eye.setPixelColor(6    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes();
  eye.setPixelColor(0, 255,255,255);
  eye.setPixelColor(1    , 255, 0, 0);
  eye.show();
  delay(del);
  cleareyes(eye);
  eye.setPixelColor(0, 255,255,255);
  eye.show();
}

void eyes_lookforward(Adafruit_NeoPixel eye) { 
  eye.setPixelColor(1, 25, 25, 25);
  eye.setPixelColor(0, 255, 255, 255);
  eye.setPixelColor(4, 25, 25, 25);
  
  eye.setPixelColor(2, 25, 0, 0);
  eye.setPixelColor(3, 25, 0, 0);
  eye.setPixelColor(5, 25, 0, 0);
  eye.setPixelColor(6, 25, 0, 0);
  eye.show();
}

void eyes_lookforward_red(Adafruit_NeoPixel eye) { 
  eye.setPixelColor(0, 255, 0, 0);
  eye.setPixelColor(1, 5, 0, 0);
  eye.setPixelColor(4, 5, 0, 0);
  eye.setPixelColor(2, 5, 0, 0);
  eye.setPixelColor(3, 5, 0, 0);
  eye.setPixelColor(5, 5, 0, 0);
  eye.setPixelColor(6, 5, 0, 0);
  eye.show();
}

void eyes_lookright(Adafruit_NeoPixel eye) {
  eye.setPixelColor(2, 255, 255, 255);
  eye.setPixelColor(3, 255, 255, 255);
  //whites
  
  eye.setPixelColor(0, 25, 0, 0);
  eye.setPixelColor(1, 25, 0, 0);
  eye.setPixelColor(4, 25, 0, 0);
  eye.setPixelColor(5, 25, 0, 0);
  eye.setPixelColor(6, 25, 0, 0);
  eye.show();
}

void eyes_lookleft(Adafruit_NeoPixel eye) {
  eye.setPixelColor(5, 255, 255, 255);
  eye.setPixelColor(6, 255, 255, 255);
  //whites
  int whitelevel = 2;
  eye.setPixelColor(0, 75, 0, 0);
  eye.setPixelColor(1, 75, 0, 0);
  eye.setPixelColor(2, 75, 0, 0);
  eye.setPixelColor(3, 75, 0, 0);
  eye.setPixelColor(4, 75, 0, 0);
  eye.show();
}


/**** debug routines ***/

// -- sweep left and right to the maximim and minimum positions as #define d
void dbg_x_srv_range() {
  for (uint16_t pulselen = X_SERVOMIN; pulselen < X_SERVOMAX; pulselen+=1) {
    Serial.print(pulselen);
    pwm.setPWM(x_servonum, 0, pulselen);
  }
  delay(5000);
  for (uint16_t pulselen = X_SERVOMAX; pulselen > X_SERVOMIN; pulselen-=1) {
    Serial.println(pulselen);
    pwm.setPWM(x_servonum, 0, pulselen);
  }
  delay(5000);
}

// -- jump to some fixed locations on horizontal x axis to ensure movement working as expected
void dbg_x_srv_fixedrange() {
  x_srv_goto(90);
  delay(2000);
  x_srv_goto(45);
  delay(2000);
  x_srv_goto(135);
  delay(2000);
  x_srv_goto(0);
  delay(2000);
  x_srv_goto(180);
  delay(2000);
}

// -- initialize the Y servo to find current location.  servo does not support moving directly
//  - to .write positions() because it is a piece of crap.  sweeps back and forth completely to latch
//  - on to last position servo was in guides it back to a fixed starting location.
void y_srv_sweepstart() {
  int angle=0;
  for (uint16_t pulselen = Y_SERVOMIN; pulselen <= Y_SERVOMAX; pulselen+=1) {
    angle=y_srv_map_angle_reverse(pulselen);
    Serial.print(pulselen);  Serial.print(" - ");
    Serial.print(angle); Serial.print(" - ");
    Serial.println(y_srv_map_angle(angle));
    
    pwm.setPWM(y_servonum, 0, pulselen);
    delay(10);
  }
  Serial.println("max..");  
  for (uint16_t pulselen = Y_SERVOMAX; pulselen >= Y_SERVOMIN; pulselen-=1) {
    angle=y_srv_map_angle_reverse(pulselen);
    Serial.print(pulselen);  Serial.print(" - ");
    Serial.print(angle); Serial.print(" - ");
    Serial.println(y_srv_map_angle(angle));
    pwm.setPWM(y_servonum, 0, pulselen);
    delay(10);
  }
  Serial.println("min");
  y_srv_pos = Y_SERVOMIN; 
}

