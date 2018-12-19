#include <CapacitiveSensor.h>

#define redPin 5
#define greenPin 9
#define bluePin 6
#define photoPin A3
#define potPin A0
#define touchRec 2
#define touchSen 4
#define modes 4
#define optionModes 2
#define potProbes 10
int startIlumination = 400;
int red, green, blue;
int col = 0;
int mode = 0;

int potPrev[potProbes] = {0};
int potCur = 0;

int photoRes = 0;

float maxPower = 255;

CapacitiveSensor capSensor = CapacitiveSensor(touchRec, touchSen);
int touchThreshold = 1000;
float touchStart;
bool wasTouched = false;

int timer = 0;
int lastPotRead = 0;

bool optionMenu = false;
int optionMode = 0;

bool isLightOn = true;

int redG = 0;
int blueG = 255;
int greenG = 0;

int allColors = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(photoPin, INPUT);
  pinMode(potPin, INPUT);
}

void loop() {
  checkTouchSensor();
  calcPot();
  calcPhoto();

  if (startIlumination > photoRes) {
    isLightOn = true;
  } else if (startIlumination < photoRes - 50) {
    isLightOn = false;
  }

  if (optionMenu) {
    optionsMenu();
  } else {
    normalMode();
  }


  delay(5);
}

void checkTouchSensor() {
  long sensorValue = capSensor.capacitiveSensor(30);
  if (sensorValue < 0) return;
  if (sensorValue > touchThreshold && !wasTouched) {
    touchStart = millis();
    wasTouched = true;
    Serial.println("Nacisnieto");
  } else if (sensorValue <= touchThreshold && wasTouched) {
    touchAction(millis() - touchStart);
    wasTouched = false;
    Serial.println("Puszczono");
  }
}

void blinkRGB() {
  isLightOn = true;
  switch (optionMenu ? optionMode : mode) {
    case 0:
      blinkColor(120, 0, 0);
      break;
    case 1:
      blinkColor(0, 120, 0);
      break;
    case 2:
      blinkColor(0, 0, 120);
      break;
  }
}

void blinkColor(int r, int g, int b) {
  setColor(r, g, b);
  delay(150);
  setColor(0, 0, 0);
  delay(150);
  setColor(r, g, b);
  delay(150);
  setColor(0, 0, 0);
  delay(150);
  setColor(r, g, b);
  delay(150);
  setColor(0, 0, 0);
  delay(150);
}

void touchAction(float t) {
  if (t > 1000) {
    optionMenu = !optionMenu;
    optionMode = 0;
    Serial.print(optionMenu);
  } else if (t > 10) {
    if (!optionMenu) {
      mode = (mode + 1) % modes;
    } else {
      optionMode = (optionMode + 1) % optionModes;
    }
  }
  blinkRGB();
}

void calcPot() {
  int cur = analogRead(potPin);
  int prev;
  for (int i = 0; i < potProbes - 1; i++) {
    potPrev[i] = potPrev[i + 1];
  }
  potPrev[potProbes - 1] = cur;
  int avr = 0;
  for (int i = 0; i < potProbes; i++) {
    avr += potPrev[i];
  }
  avr /= potProbes;
  potCur = avr;
}

void calcPhoto() {
  photoRes = analogRead(photoPin);
}

void setColor(int r, int g, int b) {
  if (isLightOn) {
    analogWrite(redPin, r * (maxPower / 255.f));
    analogWrite(greenPin, g * (maxPower / 255.f));
    analogWrite(bluePin, b * (maxPower / 255.f));
  } else {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }
}

void calculateRGB(int c) {
  if (c < 120) {
    redG = map(c, 0, 119, 255, 0);
    greenG = map(c, 0, 119, 0, 255);
    blueG = 0;
  } else if (c < 240) {
    redG = 0;
    greenG = map(c, 120, 239, 255, 0);
    blueG = map(c, 120, 239, 0, 255);
  } else {
    redG = map(c, 240, 359, 0, 255);
    greenG = 0;
    blueG = map(c, 240, 359, 255, 0);
  }
}

void optionsMenu() {
  switch (optionMode) {
    case 0:
      isLightOn = true;
      maxPower = map(potCur, 0, 1023, 0, 255);
      setColor(maxPower, 0, 0);
      break;
    case 1:
      startIlumination = potCur;
      setColor(120, 120, 120);
      break;
  }
}

void normalMode() {
  switch (mode) {
    case 0:
      isLightOn = false;
      break;
    case 1:
      oneColorMode();
      break;
    case 2:
      whiteMode();
      break;
    case 3:
      allColorMode();
      break;
  }
}

void oneColorMode() {
  int col = map(potCur, 0, 1023, 0, 359);
  calculateRGB(col);
  setColor(redG, blueG, greenG);
}

void whiteMode() {
  setColor(255, 255, 100);
}

void allColorMode(){
  allColors = (++allColors)%7200;
  int c = allColors/20;
  calculateTGB(c);
  setColor(redG, blueG, greenG);
}



