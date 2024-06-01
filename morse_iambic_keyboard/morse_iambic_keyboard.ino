#include <map>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "dictionary.h"

/*
  International Morse Code Rules
  1. The length of a dot is one unit.
  2. A dash is three units.
  3. The space between parts of the same latter is one unit.
  4. The space between letters is three units.
  5. The space between words is seven units.
*/

USBHIDKeyboard Keyboard;

/* speed */
const int dotDuration = 70;  // unit duration in milliseconds, change it according to your level
const int dashDuration = dotDuration * 3;
const int spaceDuration = dotDuration * 7;

/* symbols */
const char DOT = '.';
const char DASH = '-';

/* pins settings */
const byte dotPin = D3;
const byte dashPin = D2;
int dotState = HIGH;
int dashState = HIGH;

/* variables for program flow control */
char currentSymbol = 0;
char lastSymbol = 0;
String symbolsBuffer = "";
boolean isBusy = false;
boolean letterPrinted = false;
std::map<char, boolean> queue{
  { DOT, false },
  { DASH, false },
};
unsigned long stepTimestamp = 0;
unsigned long dotPressTimestamp = 0;
unsigned long dashPressTimestamp = 0;

void setup() {
  /* set pins with pullup so no resistors are required */
  pinMode(dotPin, INPUT_PULLUP);
  pinMode(dashPin, INPUT_PULLUP);
  /* initialize keyboard and USB */
  Keyboard.begin();
  USB.begin();
}

void loop() {
  int currentDotState = digitalRead(dotPin);
  int currentDashState = digitalRead(dashPin);

  if (dotState != currentDotState) {  // check if DOT button state has changed (pressed or released)
    dotState = currentDotState;
    if (!dotState && currentSymbol != DOT) {  // if DOT button was pressed and DOT is not processed at the moment
      queue[DOT] = true;                      // than enqueue DOT
      dotPressTimestamp = millis();
    }
  }

  if (dashState != currentDashState) {  // check if DASH button state has changed (pressed or released)
    dashState = currentDashState;
    if (!dashState && currentSymbol != DASH) {  // if DASH button was pressed and DASH is not processed at the moment
      queue[DASH] = true;                       // than enqueue DASH
      dashPressTimestamp = millis();
    }
  }

  /* select current symbol */
  if (!isBusy) {
    if (queue[DOT] && queue[DASH]) {
      /*
        both DOT and DASH buttons were pressed and enqueued,
        this is also known as "paddles squeezing"
        current symbol is set to the button which was pressed first
        and this symbol is removed from queue
      */
      currentSymbol = dotPressTimestamp < dashPressTimestamp ? DOT : DASH;
      queue[currentSymbol] = false;
    } else if (queue[DOT]) {
      if (!dashState && lastSymbol == DOT) {
        /*
          DOT button was pressed and enqueued
          but also DASH button is keep being pressed and not released
          current symbol is set with respect to alternation pattern
          that is if previous symbol was DOT than select DASH and keep DOT in queue
        */
        currentSymbol = DASH;
      } else {
        queue[DOT] = false;
        currentSymbol = DOT;
      }
    } else if (queue[DASH]) {
      if (!dotState && lastSymbol == DASH) {
        /*
          DASH button was pressed and enqueued
          but also DOT button is keep being pressed and not released
          current symbol is set with respect to alternation pattern
          that is if previous symbol was DASH than select DOT and keep DASH in queue
        */
        currentSymbol = DOT;
      } else {
        queue[DASH] = false;
        currentSymbol = DASH;
      }
    } else if (!dotState && !dashState) {
      /*
        both DASH and DOT button are keep being pressed and not released
        current symbol is set with respect to alternation pattern
        that is if previous symbol was DASH than select DOT and keep DASH in queue
      */
      currentSymbol = lastSymbol == DOT ? DASH : DOT;
    } else if (!dotState) {
      currentSymbol = DOT;  // DOT button is keep being pressed and not released
    } else if (!dashState) {
      currentSymbol = DASH;  // DASH button is keep being pressed and not released
    } else {
      currentSymbol = 0;
    }
  }

  if (isBusy) {
    if (currentSymbol == 0) {
      /*
        busy flag is active and current symbol is null
        which means that the space between parts of the same letter is in progress
        check if interval is complete
      */
      if (millis() - stepTimestamp > dotDuration) {
        isBusy = false;  // space interval is complete, remove busy flag
      }
    } else {
      /*
        busy flag is active and current symbol is present
        which means that the symbol is in progress
        check if interval is complete
      */
      if (millis() - stepTimestamp > (currentSymbol == DOT ? dotDuration : dashDuration)) {
        /*
          symbol interval is complete
          which means that symbol handling is complete
        */
        stepTimestamp = millis();                                        // update step timestamp to use it for handling the next step in sequence
        digitalWrite(currentSymbol == DOT ? LED_RED : LED_GREEN, HIGH);  // turn OFF led
        currentSymbol = 0;                                               // set current symbol to null
      }
    }
  } else if (currentSymbol != 0) {
    /*
      busy flag is not active and current symbol is present
      which means that program is ready to handle the symbol
    */
    stepTimestamp = millis();  // update step timestamp to use it for symbol handling duration
    isBusy = true;
    lastSymbol = currentSymbol;                                        // update last symbol for usage in next symbol selection
    symbolsBuffer += currentSymbol;                                    // update symbols buffer
    digitalWrite(currentSymbol == DOT ? LED_RED : LED_GREEN, LOW);     // turn ON led
    tone(A7, 550, currentSymbol == DOT ? dotDuration : dashDuration);  // play tone
  } else {
    /*
      busy flag is not active, current symbol is null
      and time since last event is three (3) units or more
      it's time to check symbols buffer
    */
    if (symbolsBuffer.length() > 0 && (millis() - stepTimestamp > dashDuration)) {
      char foundLetter = morseDictionary[symbolsBuffer.c_str()];  // get letter from dictionary

      if (foundLetter) {
        Keyboard.write(foundLetter);  // if letter found than send to keyboard
        letterPrinted = true;         // prepare for a space between words
      }

      symbolsBuffer = "";  // empty symbols buffer
    }

    /*
      busy flag is not active, current symbol is null, letter was printed
      and time since last event is seven (7) units or more
      it's time to put space between words
    */
    if (letterPrinted && millis() - stepTimestamp > spaceDuration) {
      Keyboard.write(32);
      letterPrinted = false;  // update letter flag to prevent subsequent spaces printing
    }
  }

  delay(10);
}
