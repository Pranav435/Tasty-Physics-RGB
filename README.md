# 🌈 Tasty Physics RGB

Because why settle for basic LED strips when you can have standing waves, chaotic attractors, a mode literally called **Gay Motherfucker**, and a relay that lets your light strip *gaslight* your entire room?
Tasty Physics RGB is a glorified mess of math, microcontrollers, and LEDs. It’s powered by physics equations we barely understand, made readable via a tiny OLED, and built on a Mega because Uno and Nano have the processing power of a potato chip.

---

## 🤔 What Even Is This?

This project is what happens when two people decide that normal LED animations are too sane. So instead of sticking to “rainbow fade” and “police strobe,” we made 12 modes based on physics, chaos, and vibes. Some simulate heat diffusion. Some respond to sound. One just slaps your eyes with as much gay energy as possible.

If you’re looking for “tasteful ambient mood lighting,” turn back. If you want your room to feel like a particle collider met a rave... welcome.

---

## 🧠 Hardware Requirements

> 🧨 You **must** use an **Arduino Mega 2560**.  
> Uno and Nano will scream, crash, and combust under the weight of this code. Don't do it. Don’t try. We’ve been there. It’s not pretty.

### You’ll Need:
- Arduino Mega 2560 (the only board that won’t cry)
- WS2812/NeoPixel-compatible LED strip (211 LEDs recommended, or however many your soul can handle)
- Rotary encoder (connected to pins 4 and 5, preferably one that doesn’t fall apart when you sneeze)
- Pushbutton (pin 2, for unlocking your god powers)
- Microphone module (A0, for Sound Reactive mode that judges your music taste)
- Relay module (pin 3, for Ambience mode where the lights pretend to be off but your lamp’s still haunted)
- OLED display (SSD1306, 128x32 via I2C, to tell you which fever dream you’ve entered)
- 5V power supply that doesn’t quit on you mid-rave

---

## 🧰 Software Requirements

Install the following Arduino libraries or face compilation errors so cryptic they might as well be written in ancient Latin:

- `FastLED`
- `Adafruit GFX`
- `Adafruit SSD1306`
- `OneButton`

You can get all of these from the Arduino Library Manager, or summon them via GitHub if you like doing things the hard way.

---

## 🚀 How to Upload

1. Open the `.ino` file in Arduino IDE.
2. Set board to **Arduino Mega 2560**.
3. Upload.
4. Pray.

If everything worked, your OLED will show a mode name. If nothing happens, check your wiring. If something explodes, we were never here.

---

## 🎛 Controls

- **Short press** the button: Change solid colors (only in Mode 12).
- **Long press** the button: Change modes.
- **Twist the rotary encoder**: Adjust brightness in solid mode. Yes, just like your future.

---

## 🎇 Modes Overview

| Mode # | Name               | Description |
|--------|--------------------|-------------|
| 1      | Standing Wave      | Trippy sine wave. Makes you question your eyesight. |
| 2      | Dual-Freq Beat     | Two sinusoids walk into a bar. Chaos ensues. |
| 3      | Thermal Diffusion  | It’s getting hot in here (because of simulated thermodynamics). |
| 4      | Brownian Drift     | Like particles. On drugs. |
| 5      | Lorenz Attractor   | Classic butterfly effect. Except on your wall. |
| 6      | Quantum Tunneling  | Blinks appear and vanish. Is it real? Who knows. |
| 7      | Fluid Vortex       | You’re falling into a neon black hole. |
| 8      | Plasma Flicker     | Chaos. Lightning. Vibes. |
| 9      | Doppler Sweep      | Approaches you at the speed of OH GOD MY EYES. |
| 10     | Gay Motherfucker   | Literally rainbows and strobe. You know what you signed up for. |
| 11     | Sound Reactive     | Lights respond to sound. Don’t moan near the mic. |
| 12     | Solid Color        | Pick a color. Be boring. Or aesthetic. We don’t judge. *

\* unless you choose gray.

---

## 🛠️ Customizing It (At Your Own Risk)

- Change `NUM_LEDS` to match your LED strip. Or don't, and enjoy the segfault.
- Want more modes? Add functions and update the `switch` in `loop()`. The code is readable if you squint hard enough.
- Don't touch `modeGayMotherfucker()` unless you're ready to be blinded and emotionally changed.

---

## 🤝 Credits

- **Code**: [Pranav Bhaven Savla](https://youtube.com/@imtheblindiephoenix)  
- **Hardware Engineering**: Parampalli Pavan Maiya

Together, we made this thing way too extra.

---

## 💸 Like This Project?

If you laughed, vibed, or broke a chair dancing to the Sound Reactive mode, consider donating:  
👉 [https://paypal.me/theblindiephoenix](https://paypal.me/theblindiephoenix)

It helps us keep making ridiculous things like this, and maybe one day we’ll afford a logic analyzer that isn’t older than our parents.

---

## ⚠️ Final Warning

We are not responsible for:
- Seizures
- Existential dread
- Fried Arduinos
- Neighbors complaining about “the light show at 3AM”

Use responsibly. Or irresponsibly. Honestly, we’re not your parents.

---

