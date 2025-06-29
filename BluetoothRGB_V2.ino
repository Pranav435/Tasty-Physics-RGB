#include <FastLED.h>
#include <OneButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN       6
#define NUM_LEDS      211
#define BUTTON_PIN    2
#define MIC_PIN       A0
#define RELAY_PIN    3
#define ENC_DT_PIN   4    // Data pin from encoder
#define ENC_CLK_PIN  5    // Clock pin from encoder

// Rotary Encoder vars
int  lastClkState;       // for edge-detect
uint8_t solidBrightness = 180;  // starting brightness for mode 12

// OLED display params (I2C 128×32 or 128×64)
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

CRGB leds[NUM_LEDS];
OneButton button(BUTTON_PIN, true, true);

int mode = 1;
unsigned long phase = 0;

// physics mode globals
float heatArr[NUM_LEDS];
float voltageArr[NUM_LEDS];
float xLorenz = 0.1, yLorenz = 0, zLorenz = 0;
int p1 = 0, p2 = NUM_LEDS - 1, d1 = 1, d2 = -1;
uint8_t c1 = 0, c2 = 128;
int dpos = 0, ddir = 1;
float dheu = 128;
bool tunnelActive = false;
int tunnelPos = 0;
unsigned long tunnelTime = 0;

// brownian drift hues
float hues[NUM_LEDS];
// solid-color mode setup (White → Ambience → Physics palette…)
const CRGB solidColors[] = {
  CRGB::White,          // 0: White
  CRGB::Black,          // 1: Ambience (relay only, LEDs off)
  CRGB::Red,            // 2
  CRGB::Orange,         // 3
  CRGB::Yellow,         // 4
  CRGB::Green,          // 5
  CRGB::Cyan,           // 6
  CRGB::Blue,           // 7
  CRGB::Purple,         // 8
  CRGB::Magenta,        // 9
  CRGB(255,105,180),    // 10 Hot Pink
  CRGB(128,0,255),      // 11 UV Purple
  CRGB(0,255,128),      // 12 Aurora Green
  CRGB(255,140,0),      // 13 Plasma Orange
  CRGB(75,0,130),       // 14 Cosmic Indigo
  CRGB(0,191,255),      // 15 Deep Sky Blue
  CRGB(204,255,0),      // 16 Electric Lime
  CRGB(255,215,0),      // 17 Solar Flare Gold
  CRGB(0,255,204),      // 18 Ion Teal
  CRGB(148,0,211),      // 19 Quantum Violet
  CRGB(255,20,147),     // 20 Photon Pink
  CRGB(30,144,255),     // 21 Photon Blue
  CRGB(64,224,208),     // 22 Tachyon Turquoise
  CRGB(255,255,102),    // 23 Gamma Ray Yellow
  CRGB(199,21,133),     // 24 Nebula Magenta
  CRGB(230,230,250),    // 25 Comet White
  CRGB(192,192,192),    // 26 Stellar Silver
  CRGB(128,128,128)     // 27 Subatomic Gray
};
const char* colorNames[] = {
  "White",
  "Ambience",
  "Red",
  "Orange",
  "Yellow",
  "Green",
  "Cyan",
  "Blue",
  "Purple",
  "Magenta",
  "Hot Pink",
  "UV Purple",
  "Aurora Green",
  "Plasma Orange",
  "Cosmic Indigo",
  "Deep Sky Blue",
  "Electric Lime",
  "Solar Flare Gold",
  "Ion Teal",
  "Quantum Violet",
  "Photon Pink",
  "Photon Blue",
  "Tachyon Turquoise",
  "Gamma Ray Yellow",
  "Nebula Magenta",
  "Comet White",
  "Stellar Silver",
  "Subatomic Gray"
};
const int numSolid = sizeof(solidColors) / sizeof(solidColors[0]);


int solidIndex = 0;

// human-readable mode names
const char* modeNames[] = {
  "Standing Wave",
  "Dual-Freq Beat",
  "Thermal Diffusion",
  "Brownian Drift",
  "Lorenz Attractor",
  "Quantum Tunneling",
  "Fluid Vortex",
  "Plasma Flicker",
  "Doppler Sweep",
  "Gay Motherfucker",
  "Sound Reactive",
  "Solid Color"
};

void displayModeName() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // center the mode name
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(modeNames[mode-1], 0, 0, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);
  display.println(modeNames[mode-1]);

  // if we're in Solid Color (mode 12), show the color name below
  if (mode == 12) {
    display.getTextBounds(colorNames[solidIndex], 0, 0, &x1, &y1, &w, &h);
    int16_t cx = (SCREEN_WIDTH - w) / 2;
    display.setCursor(cx, y + h + 2);
    display.println(colorNames[solidIndex]);
  }

  display.display();
}

void nextMode() {
  mode++;
  if (mode > 12) mode = 1;
  if (mode != 12) {
    digitalWrite(RELAY_PIN, HIGH   );
  }
  displayModeName();
}

void handleClick() {
  if (mode == 12) {
    solidIndex = (solidIndex + 1) % numSolid;
    displayModeName();
  }
}

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(180);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(RELAY_PIN, OUTPUT);
digitalWrite(RELAY_PIN, HIGH);  // ensure it starts off

  displayModeName();

  button.attachLongPressStart(nextMode);
  button.attachClick(handleClick);
pinMode(ENC_DT_PIN,  INPUT_PULLUP);
pinMode(ENC_CLK_PIN, INPUT_PULLUP);
lastClkState = digitalRead(ENC_CLK_PIN);
FastLED.setBrightness(solidBrightness);

  for (int i = 0; i < NUM_LEDS; i++) {
    heatArr[i]    = 0;
    voltageArr[i] = 0;
    leds[i]       = CRGB::Black;
    hues[i]       = random(256);
  }
}

void loop() {
  button.tick();
  phase++;

  if (mode == 12 && solidIndex != 1) {
  int clkState = digitalRead(ENC_CLK_PIN);
  // detect a falling edge on CLK
  if (clkState != lastClkState && clkState == LOW) {
    // direction based on DT vs CLK
    if (digitalRead(ENC_DT_PIN) != clkState) {
      solidBrightness = min((int)255, solidBrightness + 5);
    } else {
      solidBrightness = max(10,          solidBrightness - 5);
    }
    FastLED.setBrightness(solidBrightness);
  }
  lastClkState = clkState;
}

  switch (mode) {
    case 1:  modeStandingWave();       break;
    case 2:  modeDualFrequencyBeat();  break;
    case 3:  modeThermalDiffusion();   break;
    case 4:  modeBrownianDrift();      break;
    case 5:  modeLorenzAttractor();    break;
    case 6:  modeQuantumTunneling();   break;
    case 7:  modeVortex();             break;
    case 8:  modePlasmaFlicker();      break;
    case 9:  modeDopplerSweep();       break;
    case 10: modeGayMotherfucker();    break;
    case 11: modeSoundReactive();      break;
    case 12: modeSolidColor();         break;
  }

  FastLED.show();
  FastLED.delay(10);
}

// --- All your mode functions below ---

void modeStandingWave() {
  float t = phase / 200.0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float amp = sin(PI * i / (NUM_LEDS - 1) + t);
    leds[i] = CHSV(96, 255, abs(amp) * 255);
  }
}

void modeDualFrequencyBeat() {
  float t = phase / 150.0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float v = (sin(2*PI*0.15*i + t) + sin(2*PI*0.18*i + t)) * 0.5;
    leds[i] = CHSV(160, 255, (v*0.5+0.5)*255);
  }
}

void modeThermalDiffusion() {
  if (random(100) < 10) heatArr[random(NUM_LEDS)] = 1.0;
  for (int i = 0; i < NUM_LEDS; i++) {
    float l = heatArr[(i+NUM_LEDS-1)%NUM_LEDS], r = heatArr[(i+1)%NUM_LEDS];
    heatArr[i] = (l + heatArr[i] + r)/3.0 * 0.97;
    leds[i] = HeatColor(constrain(heatArr[i]*255, 0, 255));
  }
}

void modeBrownianDrift() {
  for (int i = 0; i < NUM_LEDS; i++) {
    float target = random(256);
    hues[i] += (target - hues[i]) * 0.02;
    leds[i] = CHSV((uint8_t)hues[i], 255, 255);
  }
}

void modeLorenzAttractor() {
  float dt=0.01, s=10, r=28, b=8.0/3.0;
  float dx = s*(yLorenz-xLorenz),
        dy = xLorenz*(r-zLorenz)-yLorenz,
        dz = xLorenz*yLorenz-b*zLorenz;
  xLorenz+=dx*dt; yLorenz+=dy*dt; zLorenz+=dz*dt;
  for(int i=0;i<NUM_LEDS;i++) leds[i].nscale8(240);
  int idx = constrain((int)((xLorenz+30)/60*NUM_LEDS),0,NUM_LEDS-1);
  uint8_t hue = constrain((int)((yLorenz+30)/60*255),0,255);
  leds[idx] = CHSV(hue,255,255);
}
//
void modeQuantumTunneling() {
  if (!tunnelActive && random(200)<5) {
    tunnelActive=true;
    tunnelPos=random(NUM_LEDS);
    tunnelTime=phase;
    leds[tunnelPos]=CHSV(0,255,255);
  }
  if (tunnelActive && phase-tunnelTime>5) {
    tunnelPos=(tunnelPos+random(1,NUM_LEDS/2))%NUM_LEDS;
    leds[tunnelPos]=CHSV(0,255,255);
    tunnelActive=false;
  }
  for(int i=0;i<NUM_LEDS;i++) leds[i].nscale8(200);
}

void modeVortex() {
  float t = phase/300.0, cx=sin(t)*3, cy=cos(t)*2;
  for(int i=0;i<NUM_LEDS;i++){
    float x=i-(NUM_LEDS-1)/2.0-cx, y=cy;
    float ang=atan2(y,x), rad=sqrt(x*x+y*y);
    leds[i]=CHSV((uint8_t)((ang+PI)/(2*PI)*255),255,255/(1+rad));
  }
}

void modePlasmaFlicker() {
  if (random(200)<5) voltageArr[random(NUM_LEDS)] = 1.0;
  for (int i=0;i<NUM_LEDS;i++){
    if (voltageArr[i]>0.5 && random(2000)<50) {
      int j = i + (random(2)?1:-1);
      if (j>=0 && j<NUM_LEDS) voltageArr[j]=1.0;
    }
  }
  for (int i=0;i<NUM_LEDS;i++){
    voltageArr[i] *= 0.95;
    leds[i] = CHSV(160,255,constrain(voltageArr[i]*255,0,255));
  }
}

void modeParticleCollision() {
  for(int i=0;i<NUM_LEDS;i++) leds[i].nscale8(200);
  leds[p1] = CHSV(c1,255,255);
  leds[p2] = CHSV(c2,255,255);
  p1 += d1; p2 += d2;
  if (abs(p1-p2)<=1) {
    d1=-d1; d2=-d2;
    uint8_t t=c1; c1=c2; c2=t;
  }
  c1++; c2--;
}

void modeDopplerSweep() {
  for(int i=0;i<NUM_LEDS;i++) leds[i].nscale8(200);
  leds[dpos] = CHSV((uint8_t)dheu,255,255);
  dheu = constrain(dheu + ddir*1.5,0,255);
  dpos += ddir;
  if(dpos<=0||dpos>=NUM_LEDS-1) ddir=-ddir;
}

void modeGayMotherfucker() {
  bool strobe = (phase % 2000) < 20;
  for(int i=0;i<NUM_LEDS;i++){
    if(strobe) leds[i]=CRGB::White;
    else {
      uint8_t hue = (i*255/NUM_LEDS + phase/2) & 255;
      uint8_t bri = (sin(phase/150.0 * TWO_PI)*0.5 + 0.5)*255;
      leds[i]=CHSV(hue,255,bri);
    }
  }
}

void modeSoundReactive() {
  static uint8_t smoothedBri[NUM_LEDS];

  uint8_t micVal = constrain(map(analogRead(MIC_PIN), 0, 1023, 0, 255), 0, 255);
  uint8_t baseHue = phase * 2;

  for (int i = 0; i < NUM_LEDS; i++) {
    // give each LED a bit of offset so it's not too uniform
    uint8_t localMic = micVal - random(20);
    // smooth brightness per-LED
    smoothedBri[i] = (smoothedBri[i] * 7 + localMic) / 8;

    leds[i] = CHSV(baseHue + i * 4, 255, smoothedBri[i]);
  }
}

void modeSolidColor() {
  if (solidIndex == 1) {
    // Ambience: LEDs off, relay on
    digitalWrite(RELAY_PIN, LOW);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  } else {
    // Any other color: relay off, LEDs show the color
    digitalWrite(RELAY_PIN, HIGH);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = solidColors[solidIndex];
    }
  }
}
