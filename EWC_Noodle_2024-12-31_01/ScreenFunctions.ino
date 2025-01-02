void startScreenText(int funXCoord, int funYCoord, int funTextSize, char funText[]) {
  display.setTextSize(funTextSize);
  display.setCursor(funXCoord, funYCoord);
  display.print(funText);
  display.display();
}
void continueScreenText(char funText[]) {
  display.print(funText);
  display.display();
}
void continueScreenInt(int funInt) {
  display.print(funInt);
  display.display();
}
void continueScreenHEX(int funInt) {
  display.print(funInt, HEX);
  display.display();
}

void instrumentNameOnScreen(int funXCoord, int funYCoord, int funPreviousInstrument, int funInstrument){
  display.setTextColor(SSD1306_BLACK);
  startScreenText(funXCoord, funYCoord, 1, strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[funPreviousInstrument]))));  // Necessary casts and dereferencing, just copy.
  display.setTextColor(SSD1306_WHITE);
  startScreenText(funXCoord, funYCoord, 1, strcpy_P(buffer, (char *)pgm_read_word(&(MIDIList[funInstrument]))));  // Necessary casts and dereferencing, just copy.
}

void updateModeOnScreen(int funXCoord, int funYCoord, int funPreviousMode, int funMode){
  display.setTextColor(SSD1306_BLACK);
  startScreenText(funXCoord, funYCoord, 1, modeName[funPreviousMode]);
  display.setTextColor(SSD1306_WHITE);
  startScreenText(funXCoord, funYCoord, 1, modeName[funMode]);
}

void updateIntegerOnScreen(int funXCoord, int funYCoord, int funPreviousData, int funCurrentData){
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(funXCoord, funYCoord);
  display.print(funPreviousData);
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(funXCoord, funYCoord);
  display.print(funCurrentData);
  display.display();
  for(int i=0; i<funXCoord; i++){
    Serial.print(" ");
  }
  Serial.print(funCurrentData);
  Serial.print("\n");
}

void updateVoltageOnScreen(int funXCoord, int funYCoord, int funPreviousVoltage, int funVoltage){
  display.setTextColor(SSD1306_BLACK);
  startScreenText(funXCoord, funYCoord, 1, "");
  display.print(funPreviousVoltage/10);
  display.print(".");
  display.print(funPreviousVoltage%10);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(funXCoord, funYCoord);
  display.print(funVoltage/10);
  display.print(".");
  display.print(funVoltage%10);
  continueScreenText("V");
}


void updateMenu(int funPreviousPosition, int funPosition){
  display.setTextSize(1);
  if(funPosition == 0){
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(0, 35);
    display.print(strcpy_P(buffer, (char *)pgm_read_word(&(scaleNameList[primaryScale]))));  // Necessary casts and dereferencing, just copy.
  }
  if(funPreviousPosition == 0){
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(0, 35);
    display.print(strcpy_P(buffer, (char *)pgm_read_word(&(scaleNameList[primaryScale]))));  // Necessary casts and dereferencing, just copy.
  }
  if(funPosition == 1){
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(0, 45);
    display.print(strcpy_P(buffer, (char *)pgm_read_word(&(chordNameList[primaryChord]))));  // Necessary casts and dereferencing, just copy.
  }
  if(funPreviousPosition == 1){
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(0, 45);
    display.print(strcpy_P(buffer, (char *)pgm_read_word(&(chordNameList[primaryChord]))));  // Necessary casts and dereferencing, just copy.
  }
  
  display.display();
}
void updateScaleOnScreen(int funXCoord, int funYCoord, int funScaleName){
  startScreenText(funXCoord, funYCoord, 1, strcpy_P(buffer, (char *)pgm_read_word(&(scaleNameList[funScaleName]))));  // Necessary casts and dereferencing, just copy.
}

void updateChordOnScreen(int funXCoord, int funYCoord, int funChordName){
  startScreenText(funXCoord, funYCoord, 1, strcpy_P(buffer, (char *)pgm_read_word(&(chordNameList[funChordName]))));  // Necessary casts and dereferencing, just copy.
}

void updateTextOnScreen(int funXCoord, int funYCoord, char funText[]){
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(funXCoord, funYCoord);
  display.print(funText);
  display.display();
}

void updateInverseTextOnScreen(int funXCoord, int funYCoord, char funText[]){
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(funXCoord, funYCoord);
  display.print(funText);
  display.display();
}

void incrementMenu(){
  previousCursorPosition = cursorPosition;
  cursorPosition++;
  if(cursorPosition > 1){
    cursorPosition = 0;
  }
  updateMenu(previousCursorPosition, cursorPosition);
}

void decrementMenu(){
  previousCursorPosition = cursorPosition;
  cursorPosition--;
  if(cursorPosition < 0){
    cursorPosition = 1;
  }
  updateMenu(previousCursorPosition, cursorPosition);
}

void decrementCursorSelection(){
  if (cursorPosition == 0){
    // Change to previous scale
    primaryScale--;
    if (primaryScale < 0){
      primaryScale = scaleListEntries-1;
    }
    updateScaleArray();
    updateMenu(99, cursorPosition);
  }
  if (cursorPosition == 1){
    // Change to previous chord
    primaryChord--;
    if (primaryChord < 0){
      primaryChord = chordListEntries - 1;
    }
    
    updateChordArray();
    updateMenu(99, cursorPosition);
  }
  if (cursorPosition == 2){
    // Change something about percussion
  }
}

void incrementCursorSelection(){
  if (cursorPosition == 0){
    // Change to previous scale
    primaryScale++;
    if (primaryScale > scaleListEntries-1){
      primaryScale = 0;
    }
    updateScaleArray();
    updateMenu(99, cursorPosition);
  }
  if (cursorPosition == 1){
    // Change to previous chord
    primaryChord++;
    if (primaryChord > chordListEntries - 1){
      primaryChord = 0;
    }
    updateChordArray();
    updateMenu(99, cursorPosition);
  }
  if (cursorPosition == 2){
    // Change something about percussion
  }
}
