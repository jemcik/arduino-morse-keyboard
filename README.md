## The Morse Keyboard
The idea is quite simple: connect 2 buttons (or double paddle [telegraph key](https://en.wikipedia.org/wiki/Telegraph_key)) to the Arduino board, 
connect Arduino board to your computer and use it as a keyboard.
If dots and dashes sequence is recognized, it will be decoded to an ASCII char and sent to the computer,
and it will be just the same as if you typed it on your keyboard.
This can be useful as a trainer for those, who want to learn morse code, or you can just have fun with it.

### Project is based on [Nano ESP32](https://docs.arduino.cc/hardware/nano-esp32/) board, list of parts you will need:
- Arduino board ([refer to docs](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/) to select one that that supports HID)
- 2 push buttons (or double paddle key, which is actually just 2 buttons, I used [this one](https://www.ebay.com/itm/386997549076))
- piezo capsule (it greatly helps to train proper morse code timing, I used [PKM22EPP-40](https://www.murata.com/products/productdetail?partno=PKM22EPPH4001-B0))
- USB cable to connect Arduino to your computer

### How to run:
copy `morse_iambic_keyboard` to your Arduino sketchbook folder, open it in Arduino IDE, then upload it to your board.

### Wiring:
<img width="370" alt="wiring" src="https://github.com/jemcik/arduino-morse-keyboard/assets/4085099/45395f8c-7f01-4ade-8606-d3d7793394d1">

### Actual implementation:
<img width="370" alt="implementation" src="https://github.com/jemcik/arduino-morse-keyboard/assets/4085099/a3df8dd8-782d-4f6f-8844-486ec534b3ef">

### Demo video:
https://github.com/jemcik/arduino-morse-keyboard/assets/4085099/55b28642-c19c-46ba-b66c-811083f00032
