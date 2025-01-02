void readAnalogInputs(){

  // Velocity calculation
  primaryVelocity = analogRead(velocityAnalogInput);
  primaryVelocity = map(primaryVelocity, 0, 1023, 127, 0);

  // Modulation wheel handling
  modulationWheel = analogRead(modulationAnalogInput);
  if (modulationWheel < modWheelMin){
    modWheelMin = modulationWheel;
  }
  if (modulationWheel > modWheelMax){
    modWheelMax = modulationWheel;
  }
  Serial.print(F("Modulation wheel: "));
  Serial.print(modulationWheel);
  Serial.print("\n");
  modulationWheel = map(modulationWheel, modWheelMin, modWheelMax, 0, 127);
  if(modulationWheel != modulationWheelPrevious){
    modulationWheelPrevious = modulationWheel;
    MIDICommand(0xE0 + primaryChannel, 0, modulationWheel);
    if (verboseFeedback){
      Serial.print(F("Min: "));
      Serial.print(modWheelMin);
      Serial.print(F("\tMax: "));
      Serial.print(modWheelMax);
      Serial.print("\n");
    }
  }

  // Mode selection
  updateMode();
  if (modeSelection == 5){
    panicBoolean = true;
    panicMode(panicChannel, panicSubSection);
    panicSubSection++;
    if(panicSubSection > 15){
      panicSubSection = 0;
      panicChannel++;
    }
    if(panicChannel > 15){
      panicChannel = 0;
    }
    if(panicSubSection/4 == 0){
      pcf21.digitalWrite(controllerLED0, LOW);
      pcf21.digitalWrite(controllerLED1, HIGH);
      pcf21.digitalWrite(controllerLED2, HIGH);
      pcf21.digitalWrite(controllerLED3, HIGH);
    }
    if(panicSubSection/4 == 1){
      pcf21.digitalWrite(controllerLED0, HIGH);
      pcf21.digitalWrite(controllerLED1, LOW);
      pcf21.digitalWrite(controllerLED2, HIGH);
      pcf21.digitalWrite(controllerLED3, HIGH);
    }
    if(panicSubSection/4 == 2){
      pcf21.digitalWrite(controllerLED0, HIGH);
      pcf21.digitalWrite(controllerLED1, HIGH);
      pcf21.digitalWrite(controllerLED2, LOW);
      pcf21.digitalWrite(controllerLED3, HIGH);
    }
    if(panicSubSection/4 == 3){
      pcf21.digitalWrite(controllerLED0, HIGH);
      pcf21.digitalWrite(controllerLED1, HIGH);
      pcf21.digitalWrite(controllerLED2, HIGH);
      pcf21.digitalWrite(controllerLED3, LOW);
    }
  }
  if(modeSelection != 5 && panicBoolean){
    panicBoolean = false;
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(0, 55);
    display.print("          ");
    display.display();
    Serial.print("Panic has stopped\n");
    pcf21.digitalWrite(controllerLED0, HIGH);
    pcf21.digitalWrite(controllerLED1, HIGH);
    pcf21.digitalWrite(controllerLED2, HIGH);
    pcf21.digitalWrite(controllerLED3, HIGH);
  }

  // Voltage reading
  int previousVoltageReading = voltageReading;
  voltageReading = analogRead(voltageAnalogInput);
  voltageReading = map(voltageReading, 0, 1023, 0, 50);
  if(voltageReading != previousVoltageReading){
    updateVoltageOnScreen(100, 55, previousVoltageReading, voltageReading);
  }
}

void updateMode(){
  int previousMode = modeSelection;
  modeSelection = analogRead(modeAnalogInput);
  modeSelection = calculateMode(modeSelection);
  if((modeSelection != previousMode)){
    Serial.print(F("Mode "));
    Serial.print(modeSelection);
    Serial.print("\n");
    updateModeOnScreen(0, 25, previousMode, modeSelection);
  }
}

int calculateMode(int funAnalogReading){
  if (funAnalogReading < modePotThreshold1){
    return 1;
  }
  if ((funAnalogReading > modePotThreshold1) && (funAnalogReading < modePotThreshold2)){
    return 2;
  }
  if ((funAnalogReading > modePotThreshold2) && (funAnalogReading < modePotThreshold3)){
    return 3;
  }
  if ((funAnalogReading > modePotThreshold3) && (funAnalogReading < modePotThreshold4)){
    return 4;
  }
  if (funAnalogReading > modePotThreshold4){
    return 5;
  }
}

void readIOExpanders(){
  for (uint8_t p = 0; p < 16; p++) {
    // IO expander 1
    bool currentState = !(pcf20.digitalRead(p));
    if(!(p > 7 && p <13)){
      if(buttonState[p] < currentState){
        risingEdge[p] = true;
      }else{
        risingEdge[p] = false;
      }
      if(buttonState[p] > currentState){
        fallingEdge[p] = true;
      }else{
        fallingEdge[p] = false;
      }
      buttonState[p] = currentState;
    }
    
    // IO expander 2
    if(p < 10){
      currentState = !(pcf21.digitalRead(p));
      if(buttonState[p + 16] < currentState){
        risingEdge[p + 16] = true;
      }else{
        risingEdge[p + 16] = false;
      }
      if(buttonState[p + 16] > currentState){
        fallingEdge[p + 16] = true;
      }else{
        fallingEdge[p + 16] = false;
      }
      buttonState[p + 16] = currentState;
    }
  }
  risingEdgeAction();
  fallingEdgeAction();
}

void risingEdgeAction(){
  int previousInstrument;
  for(int i=0;i<32;i++){
    if(risingEdge[i]){
//      Serial.print("Rising\t");
//      Serial.print(i);
//      Serial.print("\n");
      if(modeSelection == 1){
        switch(i){
          case paddleUp:
            primaryPitch = incrementPitch();
            MIDICommand(0x90 + primaryChannel, primaryPitch, primaryVelocity);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x90 + primaryChannel, primaryPitch + currentChord[i], primaryVelocity);
              }
            }
            pcf20.digitalWrite(connectLEDRight, LOW);
            break;
          case paddleDown:
            primaryPitch = decrementPitch();
            MIDICommand(0x90 + primaryChannel, primaryPitch, primaryVelocity);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x90 + primaryChannel, primaryPitch + currentChord[i], primaryVelocity);
              }
            }
            pcf20.digitalWrite(connectLEDLeft, LOW);
            break;
          case topPushbutton:
            previousInstrument = primaryInstrument;
            primaryInstrument++;
            if (primaryInstrument > 127){
              primaryInstrument = 0;
            }
            if (primaryInstrument < 0){
              primaryInstrument = 127;
            }
            instrumentNameOnScreen(0, 15, previousInstrument, primaryInstrument);
            if(verboseFeedback){
              Serial.print(strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[primaryInstrument]))));  // Necessary casts and dereferencing, just copy.
              Serial.print("\n");
            }
            midiSetInstrument(primaryChannel, primaryInstrument);
            break;
          case bottomPushbutton:
            previousInstrument = primaryInstrument;
            primaryInstrument--;
            if (primaryInstrument > 127){
              primaryInstrument = 0;
            }
            if (primaryInstrument < 0){
              primaryInstrument = 127;
            }
            instrumentNameOnScreen(0, 15, previousInstrument, primaryInstrument);
            if(verboseFeedback){
              Serial.print(strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[primaryInstrument]))));  // Necessary casts and dereferencing, just copy.
              Serial.print("\n");
            }
            midiSetInstrument(primaryChannel, primaryInstrument);
            break;
          case dPadUp:
            incrementMenu();
            break;
          case dPadDn:
            decrementMenu();
            break;
          case dPadLt:
            decrementCursorSelection();
            break;
          case dPadRt:
            incrementCursorSelection();
            break;
          case logoPushbutton:
            panicChannel = 0;
            panicSubSection = 0;
            break;
          default:
            
            break;
        }
      }
      if(modeSelection == 2){
        switch(i){
          case paddleUp:
            primaryPitch = hamFistIncrementPitch();
            MIDICommand(0x90 + primaryChannel, primaryPitch, primaryVelocity);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x90 + primaryChannel, primaryPitch + currentChord[i], primaryVelocity);
              }
            }
            pcf20.digitalWrite(connectLEDRight, LOW);
            break;
          case paddleDown:
            primaryPitch = hamFistDecrementPitch();
            MIDICommand(0x90 + primaryChannel, primaryPitch, primaryVelocity);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x90 + primaryChannel, primaryPitch + currentChord[i], primaryVelocity);
              }
            }
            pcf20.digitalWrite(connectLEDLeft, LOW);
            break;
          case topPushbutton:
            previousInstrument = primaryInstrument;
            primaryInstrument++;
            if (primaryInstrument > 127){
              primaryInstrument = 0;
            }
            if (primaryInstrument < 0){
              primaryInstrument = 127;
            }
            instrumentNameOnScreen(0, 15, previousInstrument, primaryInstrument);
            if(verboseFeedback){
              Serial.print(strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[primaryInstrument]))));  // Necessary casts and dereferencing, just copy.
              Serial.print("\n");
            }
            midiSetInstrument(primaryChannel, primaryInstrument);
            break;
          case bottomPushbutton:
            previousInstrument = primaryInstrument;
            primaryInstrument--;
            if (primaryInstrument > 127){
              primaryInstrument = 0;
            }
            if (primaryInstrument < 0){
              primaryInstrument = 127;
            }
            instrumentNameOnScreen(0, 15, previousInstrument, primaryInstrument);
            if(verboseFeedback){
              Serial.print(strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[primaryInstrument]))));  // Necessary casts and dereferencing, just copy.
              Serial.print("\n");
            }
            midiSetInstrument(primaryChannel, primaryInstrument);
            break;
          case dPadUp:
            incrementMenu();
            break;
          case dPadDn:
            decrementMenu();
            break;
          case dPadLt:
            decrementCursorSelection();
            break;
          case dPadRt:
            incrementCursorSelection();
            break;
          case logoPushbutton:
            panicChannel = 0;
            panicSubSection = 0;
            break;
          default:
            
            break;
        }
      }
    }
  }
}

void fallingEdgeAction(){
  for(int i=0;i<32;i++){
    if(fallingEdge[i]){
      if(modeSelection == 1 || modeSelection == 2){
        switch(i){
          case paddleUp:
            MIDICommand(0x80 + primaryChannel, primaryPitch, 0x00);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x80 + primaryChannel, primaryPitch + currentChord[i], 0);
              }
            }
            pcf20.digitalWrite(connectLEDRight, HIGH);
            break;
          case paddleDown:
            MIDICommand(0x80 + primaryChannel, primaryPitch, 0x00);
            for(int i=1;i<4;i++){
              if(currentChord[i] != 0){
                MIDICommand(0x80 + primaryChannel, primaryPitch + currentChord[i], 0);
              }
            }
            pcf20.digitalWrite(connectLEDLeft, HIGH);
            break;
          default:
            
            break;
        }
      }
    }
  }
}
