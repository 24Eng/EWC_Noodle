int incrementPitch(){
  int proposedPitch = 0;
  bool buttonPressed = true;
  if(buttonState[neckGreen] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[10]);
  }
  if(buttonState[neckRed] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[9]);
  }
  if(buttonState[neckYellow] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[8]);
  }
  if(buttonState[neckBlue] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[7]);
  }
  if(buttonState[neckOrange] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[6]);
  }
  if(proposedPitch < 0){
    proposedPitch = 0;
  }
  if(proposedPitch > 127){
    proposedPitch = 127;
  }
  if(buttonPressed){
    return primaryPitch;
  }
  return proposedPitch;
}

int decrementPitch(){
  int proposedPitch = 0;
  bool buttonPressed = true;
  // Change notes according to a scale
  if(buttonState[neckGreen] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[0]);
  }
  if(buttonState[neckRed] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[1]);
  }
  if(buttonState[neckYellow] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[2]);
  }
  if(buttonState[neckBlue] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[3]);
  }
  if(buttonState[neckOrange] && buttonPressed){
    buttonPressed = false;
    proposedPitch = (primaryPitch + currentScale[4]);
  }
  if(proposedPitch < 0){
    proposedPitch = 0;
  }
  if(proposedPitch > 127){
    proposedPitch = 127;
  }
  if(buttonPressed){
    return primaryPitch;
  }
  return proposedPitch;
}


int hamFistIncrementPitch(){
  int proposedPitch;
  // Ham-fisted chording to change notes
  int funHamDifference = calculateHamFistedChord();
  proposedPitch = primaryPitch + funHamDifference;
  if(proposedPitch < 0){
    proposedPitch = 0;
  }
  if(proposedPitch > 127){
    proposedPitch = 127;
  }
  return proposedPitch;
}

int hamFistDecrementPitch(){
  int proposedPitch;
  // Ham-fisted chording to change notes
  int funHamDifference = calculateHamFistedChord();
  proposedPitch = primaryPitch - funHamDifference;
  if(proposedPitch < 0){
    proposedPitch = 0;
  }
  if(proposedPitch > 127){
    proposedPitch = 127;
  }
  return proposedPitch;
}

int calculateHamFistedChord(){
  int funRunningTotal = 0;

  // Change by one button
  if(buttonState[neckOrange]){
    funRunningTotal = 1;
  }
  if(buttonState[neckBlue]){
    funRunningTotal = 2;
  }
  if(buttonState[neckYellow]){
    funRunningTotal = 3;
  }
  if(buttonState[neckRed]){
    funRunningTotal = 4;
  }
  if(buttonState[neckGreen]){
    funRunningTotal = 5;
  }

  // Change by two buttons
  if(buttonState[neckOrange] && buttonState[neckBlue]){
    funRunningTotal = 6;
  }
  if(buttonState[neckBlue] && buttonState[neckYellow]){
    funRunningTotal = 7;
  }
  if(buttonState[neckYellow] && buttonState[neckRed]){
    funRunningTotal = 8;
  }
  if(buttonState[neckRed] && buttonState[neckGreen]){
    funRunningTotal = 9;
  }

  // Change by three buttons
  if(buttonState[neckOrange] && buttonState[neckBlue] && buttonState[neckYellow]){
    funRunningTotal = 10;
  }
  if(buttonState[neckBlue] && buttonState[neckYellow] && buttonState[neckRed]){
    funRunningTotal = 11;
  }
  if(buttonState[neckYellow] && buttonState[neckRed] && buttonState[neckGreen]){
    funRunningTotal = 12;
  }
  
  Serial.print("Calculated pitch change:\t");
  Serial.print(funRunningTotal);
  Serial.print("\n");
  return (funRunningTotal);
}

// The following are functions from Adafruit
void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
//  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;
  
  Serial1.write(0xC0 | chan);  
  delay(10);
  Serial1.write(inst);
  delay(10);
  if(false){
    Serial.print(F("Instrument: "));
    Serial.print(chan);
    Serial.print(", ");
    Serial.print(inst);
    Serial.print("\n");
  }
}

void MIDICommand(uint8_t funCommand, uint8_t n, uint8_t vel) {
  if (n > 127){
    Serial.print(F("Note out of range\n"));
    return;
  }
  if (vel > 127){
    Serial.print(F("Velocity out of range\n"));
    return;
  }
  
  Serial1.write(funCommand);
  Serial1.write(n);
  Serial1.write(vel);
}

void panicMode(int funChannel, int funSubSection){
  int subSectionStart = funSubSection * 8;
  for (int i=0; i<8; i++){
    MIDICommand(0x80 + funChannel, i + subSectionStart, 0);
  }
  // Show the channel currently being PANIC'd
  if(funChannel > 0){
    updateIntegerOnScreen(0, 55, funChannel-1, funChannel);
  }else{
    updateIntegerOnScreen(0, 55, 15, funChannel);
  }
  // Show the subsection currently being PANIC'd
  if(funSubSection > 0){
    updateIntegerOnScreen(15, 55, (funSubSection-1)*8, funSubSection*8);
  }else{
    updateIntegerOnScreen(15, 55, 15*8, funSubSection*8);
  }
}

void updateScaleArray(){
  for (int i = 0; i < 11; i++){
    if(primaryScale == 0){
      currentScale[i] = minorPentatonicScale[i];
    }
    if(primaryScale == 1){
      currentScale[i] = majorPentatonicScale[i];
    }
    if(primaryScale == 2){
      currentScale[i] = EgyptianPentatonicScale[i];
    }
    if(primaryScale == 3){
      currentScale[i] = bluesMinorPentatonicScale[i];
    }
    if(primaryScale == 4){
      currentScale[i] = bluesMajorPentatonicScale[i];
    }
  }
}


void updateChordArray(){
  for (int i = 0; i < 4; i++){
    if(primaryChord == 0){
      currentChord[i] = 0;
    }
    if(primaryChord == 1){
      currentChord[i] = majorTriadChord[i];
    }
    if(primaryChord == 2){
      currentChord[i] = majorSixthChord[i];
    }
    if(primaryChord == 3){
      currentChord[i] = dominantSeventhChord[i];
    }
    if(primaryChord == 4){
      currentChord[i] = majorSeventhChord[i];
    }
    if(primaryChord == 5){
      currentChord[i] = augmentedTriad[i];
    }
    if(primaryChord == 6){
      currentChord[i] = augmentedSeventhChord[i];
    }
    if(primaryChord == 7){
      currentChord[i] = minorTriad[i];
    }
    if(primaryChord == 8){
      currentChord[i] = minorSixthChord[i];
    }
    if(primaryChord == 9){
      currentChord[i] = minorSeventhChord[i];
    }
    if(primaryChord == 10){
      currentChord[i] = minorMajorSeventhChord[i];
    }
    if(primaryChord == 11){
      currentChord[i] = diminishedTriadChord[i];
    }
    if(primaryChord == 12){
      currentChord[i] = diminishedSeventhChord[i];
    }
    if(primaryChord == 13){
      currentChord[i] = halfDiminishedSeventhChord[i];
    }
    Serial.print(currentChord[i]);
    Serial.print("\t");
  }
  Serial.print("\n");
}
