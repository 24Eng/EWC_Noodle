/*
  Example code from Adafruit.
  The rest is from Brian McEvoy, https://www.24hourengineer.com/
  Code was designed for an Arduino Micro.
*/

#include <Adafruit_PCF8575.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_PCF8575 pcf20;
Adafruit_PCF8575 pcf21;

// Global variables
int primaryPitch = 60;
int primaryVelocity = 0;
int primaryChannel = 0;
int primaryInstrument = 0;
int primaryChord = 0;
int primaryScale = 0;
int cursorPosition = 0;
int previousCursorPosition = 99;
bool verboseFeedback = 1;
int modulationWheel = 0;
int modulationWheelPrevious = 0;
int modWheelMin = 500;
int modWheelMax = 0;
int modeSelection = 0;
int voltageReading = 0;
int panicChannel = 0;
int panicSubSection = 0;
bool panicBoolean = false;
// These are hard-coded values based on my guitar controller so
// you probably have to change them according the values you see when
// reading the input.
const int modePotThreshold1 = 235;
const int modePotThreshold2 = 460;
const int modePotThreshold3 = 670;
const int modePotThreshold4 = 895;
char modeName[][16] = {"", "Scales", "Ham-Fisted", "Mode three", "Mode four", "PANIC MODE"};

// IO declarations
int modulationAnalogInput = 0;
int velocityAnalogInput = 1;
int modeAnalogInput = 2;
int voltageAnalogInput = 3;
bool buttonState[32];
bool risingEdge[32];
bool fallingEdge[32];

// IO connections as they relate to the buttonState[] array
const int bottomPushbutton = 0;
const int topPushbutton = 1;
const int dPadLt = 2;
const int dPadUp = 3;
const int dPadDn = 4;
const int dPadRt = 5;
const int paddleDown = 6;
const int paddleUp = 7;

const int connectLEDLeft = 11;
const int connectLEDRight = 12;

const int logoPushbutton = 13;
const int connectPushbutton = 14;

const int baseGreen = 16;
const int baseRed = 17;
const int baseYellow = 18;
const int baseBlue = 19;
const int baseOrange = 20;
const int neckOrange = 21;
const int neckBlue = 22;
const int neckYellow = 23;
const int neckRed = 24;
const int neckGreen = 25;

const int controllerLED0 = 28-16;
const int controllerLED1 = 29-16;
const int controllerLED2 = 30-16;
const int controllerLED3 = 31-16;

// Scales
const int minorPentatonicScale[] = {-12, -9, -8, -5, -2, 0, 3, 4, 7, 10, 12};
const int majorPentatonicScale[] = {-12, -10, -8, -5, -3, 0, 2, 4, 7, 9, 12};
const int EgyptianPentatonicScale[] = {-12, -10, -7, -5, -2, 0, 2, 5, 7, 10, 12};
const int bluesMinorPentatonicScale[] = {-12, -9, -7, -4, -2, 0, 3, 5, 8, 10, 12};
const int bluesMajorPentatonicScale[] = {-12, -10, -7, -5, -3, 0, 2, 5, 7, 9, 12};

const char scale_001[] PROGMEM = "Minor Pentatonic   ";
const char scale_002[] PROGMEM = "Major Pentatonic   ";
const char scale_003[] PROGMEM = "Egyptian Pentatonic";
const char scale_004[] PROGMEM = "Blues Minor        ";
const char scale_005[] PROGMEM = "Blues Major        ";
const int scaleListEntries = 5;
const char *const scaleNameList[scaleListEntries] PROGMEM = {scale_001, scale_002, scale_003, scale_004, scale_005};

int currentScale[11] = {-12, -9, -8, -5, -2, 0, 3, 4, 7, 10, 12};

// Chords
const int majorTriadChord[] = {0, 4, 7, 0};
const int majorSixthChord[] = {0, 4, 7, 9};
const int dominantSeventhChord[] = {0, 4, 7, 10};
const int majorSeventhChord[] = {0, 4, 7, 11};
const int augmentedTriad[] = {0, 4, 8, 0};
const int augmentedSeventhChord[] = {0, 4, 8, 10};
const int minorTriad[] = {0, 3, 7, 0};
const int minorSixthChord[] = {0, 3, 7, 9};
const int minorSeventhChord[] = {0, 3, 7, 10};
const int minorMajorSeventhChord[] = {0, 3, 7, 11};
const int diminishedTriadChord[] = {0, 3, 6, 0};
const int diminishedSeventhChord[] = {0, 3, 6, 9};
const int halfDiminishedSeventhChord[] = {0, 3, 6, 10};

const char chord_000[] PROGMEM = "No Chord          ";
const char chord_001[] PROGMEM = "Major Triad       ";
const char chord_002[] PROGMEM = "Major 6th         ";
const char chord_003[] PROGMEM = "Dominant 7th      ";
const char chord_004[] PROGMEM = "Major 7th         ";
const char chord_005[] PROGMEM = "Augmented Triad   ";
const char chord_006[] PROGMEM = "Augmented 7th     ";
const char chord_007[] PROGMEM = "Minor Triad       ";
const char chord_008[] PROGMEM = "Minor 6th         ";
const char chord_009[] PROGMEM = "Minor 7th         ";
const char chord_010[] PROGMEM = "Minor Major 7th   ";
const char chord_011[] PROGMEM = "Diminished Triad  ";
const char chord_012[] PROGMEM = "Diminished 7th    ";
const char chord_013[] PROGMEM = "1/2 Diminished 7th";
const int chordListEntries = 14;
const char *const chordNameList[chordListEntries] PROGMEM = {chord_000, chord_001, chord_002, chord_003, chord_004, chord_005, chord_006, chord_007, chord_008, chord_009, chord_010, chord_011, chord_012, chord_013};

int currentChord[4] = {0};

// MIDI instrument names
const char percussion_027[] PROGMEM = "          High Q";
const char percussion_028[] PROGMEM = "            Slap";
const char percussion_029[] PROGMEM = "    Scratch Push";
const char percussion_030[] PROGMEM = "    Scratch Pull";
const char percussion_031[] PROGMEM = "          Sticks";
const char percussion_032[] PROGMEM = "    Square Click";
const char percussion_033[] PROGMEM = " Metronome Click";
const char percussion_034[] PROGMEM = "  Metronome Bell";
const char percussion_035[] PROGMEM = "     Bass Drum 2";
const char percussion_036[] PROGMEM = "     Bass Drum 1";
const char percussion_037[] PROGMEM = "      Side Stick";
const char percussion_038[] PROGMEM = "    Snare Drum 1";
const char percussion_039[] PROGMEM = "       Hand Clap";
const char percussion_040[] PROGMEM = "    Snare Drum 2";
const char percussion_041[] PROGMEM = "       Low Tom 2";
const char percussion_042[] PROGMEM = "   Closed Hi-hat";
const char percussion_043[] PROGMEM = "       Low Tom 1";
const char percussion_044[] PROGMEM = "    Pedal Hi-hat";
const char percussion_045[] PROGMEM = "       Mid Tom 2";
const char percussion_046[] PROGMEM = "     Open Hi-hat";
const char percussion_047[] PROGMEM = "       Mid Tom 1";
const char percussion_048[] PROGMEM = "      High Tom 2";
const char percussion_049[] PROGMEM = "  Crash Cymbal 1";
const char percussion_050[] PROGMEM = "      High Tom 1";
const char percussion_051[] PROGMEM = "   Ride Cymbal 1";
const char percussion_052[] PROGMEM = "  Chinese Cymbal";
const char percussion_053[] PROGMEM = "       Ride Bell";
const char percussion_054[] PROGMEM = "      Tambourine";
const char percussion_055[] PROGMEM = "   Splash Cymbal";
const char percussion_056[] PROGMEM = "         Cowbell";
const char percussion_057[] PROGMEM = "  Crash Cymbal 2";
const char percussion_058[] PROGMEM = "      Vibra Slap";
const char percussion_059[] PROGMEM = "   Ride Cymbal 2";
const char percussion_060[] PROGMEM = "      High Bongo";
const char percussion_061[] PROGMEM = "       Low Bongo";
const char percussion_062[] PROGMEM = " Mute High Conga";
const char percussion_063[] PROGMEM = " Open High Conga";
const char percussion_064[] PROGMEM = "       Low Conga";
const char percussion_065[] PROGMEM = "    High Timbale";
const char percussion_066[] PROGMEM = "     Low Timbale";
const char percussion_067[] PROGMEM = "      High Agogo";
const char percussion_068[] PROGMEM = "       Low Agogo";
const char percussion_069[] PROGMEM = "          Cabasa";
const char percussion_070[] PROGMEM = "         Maracas";
const char percussion_071[] PROGMEM = "   Short Whistle";
const char percussion_072[] PROGMEM = "    Long Whistle";
const char percussion_073[] PROGMEM = "     Short Guiro";
const char percussion_074[] PROGMEM = "      Long Guiro";
const char percussion_075[] PROGMEM = "          Claves";
const char percussion_076[] PROGMEM = " High Wood Block";
const char percussion_077[] PROGMEM = "  Low Wood Block";
const char percussion_078[] PROGMEM = "      Mute Cuica";
const char percussion_079[] PROGMEM = "      Open Cuica";
const char percussion_080[] PROGMEM = "   Mute Triangle";
const char percussion_081[] PROGMEM = "   Open Triangle";
const char percussion_082[] PROGMEM = "          Shaker";
const char percussion_083[] PROGMEM = "     Jingle Bell";
const char percussion_084[] PROGMEM = "        Belltree";
const char percussion_085[] PROGMEM = "       Castanets";
const char percussion_086[] PROGMEM = "      Mute Surdo";
const char percussion_087[] PROGMEM = "      Open Surdo";

const char *const MIDIPercussionList[61] PROGMEM = {
    percussion_027,
    percussion_028,
    percussion_029,
    percussion_030,
    percussion_031,
    percussion_032,
    percussion_033,
    percussion_034,
    percussion_035,
    percussion_036,
    percussion_037,
    percussion_038,
    percussion_039,
    percussion_040,
    percussion_041,
    percussion_042,
    percussion_043,
    percussion_044,
    percussion_045,
    percussion_046,
    percussion_047,
    percussion_048,
    percussion_049,
    percussion_050,
    percussion_051,
    percussion_052,
    percussion_053,
    percussion_054,
    percussion_055,
    percussion_056,
    percussion_057,
    percussion_058,
    percussion_059,
    percussion_060,
    percussion_061,
    percussion_062,
    percussion_063,
    percussion_064,
    percussion_065,
    percussion_066,
    percussion_067,
    percussion_068,
    percussion_069,
    percussion_070,
    percussion_071,
    percussion_072,
    percussion_073,
    percussion_074,
    percussion_075,
    percussion_076,
    percussion_077,
    percussion_078,
    percussion_079,
    percussion_080,
    percussion_081,
    percussion_082,
    percussion_083,
    percussion_084,
    percussion_085,
    percussion_086,
    percussion_087
};


const char instrument_000[] PROGMEM = "Acou Grand Piano";
const char instrument_001[] PROGMEM = " Brt Acou Piano ";
const char instrument_002[] PROGMEM = "Elec Grand Piano";
const char instrument_003[] PROGMEM = "Honky-tonk Piano";
const char instrument_004[] PROGMEM = "Electric Piano 1";
const char instrument_005[] PROGMEM = "Electric Piano 2";
const char instrument_006[] PROGMEM = "  Harpsichord   ";
const char instrument_007[] PROGMEM = "    Clavinet    ";
const char instrument_008[] PROGMEM = "    Celesta     ";
const char instrument_009[] PROGMEM = "  Glockenspiel  ";
const char instrument_010[] PROGMEM = "   Music Box    ";
const char instrument_011[] PROGMEM = "   Vibraphone   ";
const char instrument_012[] PROGMEM = "    Marimba     ";
const char instrument_013[] PROGMEM = "   Xylophone    ";
const char instrument_014[] PROGMEM = " Tubular Bells  ";
const char instrument_015[] PROGMEM = "    Dulcimer    ";
const char instrument_016[] PROGMEM = " Drawbar Organ  ";
const char instrument_017[] PROGMEM = "Percussive Organ";
const char instrument_018[] PROGMEM = "   Rock Organ   ";
const char instrument_019[] PROGMEM = "  Church Organ  ";
const char instrument_020[] PROGMEM = "   Reed Organ   ";
const char instrument_021[] PROGMEM = "   Accordion    ";
const char instrument_022[] PROGMEM = "   Harmonica    ";
const char instrument_023[] PROGMEM = "Tango Accordion ";
const char instrument_024[] PROGMEM = "Ac Guitar(Nylon)";
const char instrument_025[] PROGMEM = "Ac Guitar(Steel)";
const char instrument_026[] PROGMEM = "Elc Guitar(Jazz)";
const char instrument_027[] PROGMEM = "Ec Guitar(Clean)";
const char instrument_028[] PROGMEM = "Ec Guitar(Muted)";
const char instrument_029[] PROGMEM = "Overdrive Guitar";
const char instrument_030[] PROGMEM = "  Dist Guitar   ";
const char instrument_031[] PROGMEM = "Guitar Harmonics";
const char instrument_032[] PROGMEM = " Acoustic Bass  ";
const char instrument_033[] PROGMEM = "Ec Bass (Finger)";
const char instrument_034[] PROGMEM = " Ec Bass (Pick) ";
const char instrument_035[] PROGMEM = " Fretless Bass  ";
const char instrument_036[] PROGMEM = "  Slap Bass 1   ";
const char instrument_037[] PROGMEM = "  Slap Bass 2   ";
const char instrument_038[] PROGMEM = "  Synth Bass 1  ";
const char instrument_039[] PROGMEM = "  Synth Bass 2  ";
const char instrument_040[] PROGMEM = "     Violin     ";
const char instrument_041[] PROGMEM = "     Viola      ";
const char instrument_042[] PROGMEM = "     Cello      ";
const char instrument_043[] PROGMEM = "   Contrabass   ";
const char instrument_044[] PROGMEM = "Tremolo Strings ";
const char instrument_045[] PROGMEM = "Pizzicato Stngs ";
const char instrument_046[] PROGMEM = "Orchestral Harp ";
const char instrument_047[] PROGMEM = "    Timpani     ";
const char instrument_048[] PROGMEM = "String Ensemble1";
const char instrument_049[] PROGMEM = "String Ensemble2";
const char instrument_050[] PROGMEM = "Synth Strings 1 ";
const char instrument_051[] PROGMEM = "Synth Strings 2 ";
const char instrument_052[] PROGMEM = "   Choir Aahs   ";
const char instrument_053[] PROGMEM = "   Voice Oohs   ";
const char instrument_054[] PROGMEM = "  Synth Choir   ";
const char instrument_055[] PROGMEM = " Orchestra Hit  ";
const char instrument_056[] PROGMEM = "    Trumpet     ";
const char instrument_057[] PROGMEM = "    Trombone    ";
const char instrument_058[] PROGMEM = "      Tuba      ";
const char instrument_059[] PROGMEM = " Muted Trumpet  ";
const char instrument_060[] PROGMEM = "  French Horn   ";
const char instrument_061[] PROGMEM = " Brass Section  ";
const char instrument_062[] PROGMEM = " Synth Brass 1  ";
const char instrument_063[] PROGMEM = " Synth Brass 2  ";
const char instrument_064[] PROGMEM = "  Soprano Sax   ";
const char instrument_065[] PROGMEM = "    Alto Sax    ";
const char instrument_066[] PROGMEM = "   Tenor Sax    ";
const char instrument_067[] PROGMEM = "  Baritone Sax  ";
const char instrument_068[] PROGMEM = "      Oboe      ";
const char instrument_069[] PROGMEM = "  English Horn  ";
const char instrument_070[] PROGMEM = "    Bassoon     ";
const char instrument_071[] PROGMEM = "    Clarinet    ";
const char instrument_072[] PROGMEM = "    Piccolo     ";
const char instrument_073[] PROGMEM = "     Flute      ";
const char instrument_074[] PROGMEM = "    Recorder    ";
const char instrument_075[] PROGMEM = "   Pan Flute    ";
const char instrument_076[] PROGMEM = "  Blown bottle  ";
const char instrument_077[] PROGMEM = "   Shakuhachi   ";
const char instrument_078[] PROGMEM = "    Whistle     ";
const char instrument_079[] PROGMEM = "    Ocarina     ";
const char instrument_080[] PROGMEM = "Lead 1 (Square) ";
const char instrument_081[] PROGMEM = "Lead 2(Sawtooth)";
const char instrument_082[] PROGMEM = "Lead 3(Calliope)";
const char instrument_083[] PROGMEM = " Lead 4 (Chiff) ";
const char instrument_084[] PROGMEM = "Lead 5 (Charang)";
const char instrument_085[] PROGMEM = " Lead 6 (Voice) ";
const char instrument_086[] PROGMEM = "Lead 7 (Fifths) ";
const char instrument_087[] PROGMEM = "Lead8(Bass+Lead)";
const char instrument_088[] PROGMEM = "Pad 1 (New Age) ";
const char instrument_089[] PROGMEM = "  Pad 2 (Warm)  ";
const char instrument_090[] PROGMEM = "Pad 3(Polysynth)";
const char instrument_091[] PROGMEM = " Pad 4 (Choir)  ";
const char instrument_092[] PROGMEM = " Pad 5 (Bowed)  ";
const char instrument_093[] PROGMEM = "Pad 6 (Metallic)";
const char instrument_094[] PROGMEM = "  Pad 7 (Halo)  ";
const char instrument_095[] PROGMEM = " Pad 8 (Sweep)  ";
const char instrument_096[] PROGMEM = "  FX 1 (Rain)   ";
const char instrument_097[] PROGMEM = "FX 2(Soundtrack)";
const char instrument_098[] PROGMEM = " FX 3 (Crystal) ";
const char instrument_099[] PROGMEM = "FX 4(Atmosphere)";
const char instrument_100[] PROGMEM = "FX 5(Brightness)";
const char instrument_101[] PROGMEM = " FX 6 (Goblins) ";
const char instrument_102[] PROGMEM = " FX 7 (Echoes)  ";
const char instrument_103[] PROGMEM = " FX 8 (Sci-fi)  ";
const char instrument_104[] PROGMEM = "     Sitar      ";
const char instrument_105[] PROGMEM = "     Banjo      ";
const char instrument_106[] PROGMEM = "    Shamisen    ";
const char instrument_107[] PROGMEM = "      Koto      ";
const char instrument_108[] PROGMEM = "    Kalimba     ";
const char instrument_109[] PROGMEM = "    Bagpipe     ";
const char instrument_110[] PROGMEM = "     Fiddle     ";
const char instrument_111[] PROGMEM = "     Shanai     ";
const char instrument_112[] PROGMEM = "  Tinkle Bell   ";
const char instrument_113[] PROGMEM = "     Agogo      ";
const char instrument_114[] PROGMEM = "  Steel Drums   ";
const char instrument_115[] PROGMEM = "   Woodblock    ";
const char instrument_116[] PROGMEM = "   Taiko Drum   ";
const char instrument_117[] PROGMEM = "  Melodic Tom   ";
const char instrument_118[] PROGMEM = "   Synth Drum   ";
const char instrument_119[] PROGMEM = " Reverse Cymbal ";
const char instrument_120[] PROGMEM = "  Guitar Fret   ";
const char instrument_121[] PROGMEM = "  Breath Noise  ";
const char instrument_122[] PROGMEM = "    Seashore    ";
const char instrument_123[] PROGMEM = "   Bird Tweet   ";
const char instrument_124[] PROGMEM = " Telephone Ring ";
const char instrument_125[] PROGMEM = "   Helicopter   ";
const char instrument_126[] PROGMEM = "    Applause    ";
const char instrument_127[] PROGMEM = "    Gunshot     ";

const char *const MIDIList[128] PROGMEM = {
  instrument_000,
  instrument_001,
  instrument_002,
  instrument_003,
  instrument_004,
  instrument_005,
  instrument_006,
  instrument_007,
  instrument_008,
  instrument_009,
  instrument_010,
  instrument_011,
  instrument_012,
  instrument_013,
  instrument_014,
  instrument_015,
  instrument_016,
  instrument_017,
  instrument_018,
  instrument_019,
  instrument_020,
  instrument_021,
  instrument_022,
  instrument_023,
  instrument_024,
  instrument_025,
  instrument_026,
  instrument_027,
  instrument_028,
  instrument_029,
  instrument_030,
  instrument_031,
  instrument_032,
  instrument_033,
  instrument_034,
  instrument_035,
  instrument_036,
  instrument_037,
  instrument_038,
  instrument_039,
  instrument_040,
  instrument_041,
  instrument_042,
  instrument_043,
  instrument_044,
  instrument_045,
  instrument_046,
  instrument_047,
  instrument_048,
  instrument_049,
  instrument_050,
  instrument_051,
  instrument_052,
  instrument_053,
  instrument_054,
  instrument_055,
  instrument_056,
  instrument_057,
  instrument_058,
  instrument_059,
  instrument_060,
  instrument_061,
  instrument_062,
  instrument_063,
  instrument_064,
  instrument_065,
  instrument_066,
  instrument_067,
  instrument_068,
  instrument_069,
  instrument_070,
  instrument_071,
  instrument_072,
  instrument_073,
  instrument_074,
  instrument_075,
  instrument_076,
  instrument_077,
  instrument_078,
  instrument_079,
  instrument_080,
  instrument_081,
  instrument_082,
  instrument_083,
  instrument_084,
  instrument_085,
  instrument_086,
  instrument_087,
  instrument_088,
  instrument_089,
  instrument_090,
  instrument_091,
  instrument_092,
  instrument_093,
  instrument_094,
  instrument_095,
  instrument_096,
  instrument_097,
  instrument_098,
  instrument_099,
  instrument_100,
  instrument_101,
  instrument_102,
  instrument_103,
  instrument_104,
  instrument_105,
  instrument_106,
  instrument_107,
  instrument_108,
  instrument_109,
  instrument_110,
  instrument_111,
  instrument_112,
  instrument_113,
  instrument_114,
  instrument_115,
  instrument_116,
  instrument_117,
  instrument_118,
  instrument_119,
  instrument_120,
  instrument_121,
  instrument_122,
  instrument_123,
  instrument_124,
  instrument_125,
  instrument_126,
  instrument_127
};
char buffer[16];



void setup() {
  // Start the serial ports
  Serial1.begin(31250);
  Serial.begin(115200);
  Serial.print(F("EWC_Noodle\n"));
  Serial.print(F("Starting\t."));

  midiSetInstrument(primaryChannel, primaryInstrument);

  Serial.print("\t.");

  // Check or the two IO expanders
  if (!pcf20.begin(0x20, &Wire)) {
    Serial.println(F("Couldn't find PCF8575 at 0x20"));
    while (1);
  }
  Serial.print("\t.");
  if (!pcf21.begin(0x21, &Wire)) {
    Serial.print(F("Couldn't find PCF8575 at 0x21\n"));
    while (1);
  }
  // Declare the inputs and outputs
  Serial.print("\t.");
  pcf20.pinMode(topPushbutton, INPUT_PULLUP);
  pcf20.pinMode(bottomPushbutton, INPUT_PULLUP);
  pcf20.pinMode(dPadLt, INPUT_PULLUP);
  pcf20.pinMode(dPadUp, INPUT_PULLUP);
  pcf20.pinMode(dPadDn, INPUT_PULLUP);
  pcf20.pinMode(dPadRt, INPUT_PULLUP);
  pcf20.pinMode(paddleDown, INPUT_PULLUP);
  pcf20.pinMode(paddleUp, INPUT_PULLUP);
  pcf20.pinMode(logoPushbutton, INPUT_PULLUP);
  pcf20.pinMode(connectPushbutton, INPUT_PULLUP);
  pcf20.pinMode(connectLEDLeft, OUTPUT);
  pcf20.pinMode(connectLEDRight, OUTPUT);
  
  pcf21.pinMode(baseGreen - 16, INPUT_PULLUP);
  pcf21.pinMode(baseRed - 16, INPUT_PULLUP);
  pcf21.pinMode(baseYellow - 16, INPUT_PULLUP);
  pcf21.pinMode(baseBlue - 16, INPUT_PULLUP);
  pcf21.pinMode(baseOrange - 16, INPUT_PULLUP);
  pcf21.pinMode(neckOrange - 16, INPUT_PULLUP);
  pcf21.pinMode(neckBlue - 16, INPUT_PULLUP);
  pcf21.pinMode(neckYellow - 16, INPUT_PULLUP);
  pcf21.pinMode(neckRed - 16, INPUT_PULLUP);
  pcf21.pinMode(neckGreen - 16, INPUT_PULLUP);
  pcf21.pinMode(controllerLED0, OUTPUT);
  pcf21.pinMode(controllerLED1, OUTPUT);
  pcf21.pinMode(controllerLED2, OUTPUT);
  pcf21.pinMode(controllerLED3, OUTPUT);
  
  Serial.print("\t.");
  for (uint8_t p = 0; p < 16; p++) {
    pcf21.pinMode(p, INPUT_PULLUP);
  }
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  Serial.print("\t.");
  // Start the screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Serial.print("\t.");
  display.clearDisplay();
  display.display();
  Serial.print("\t.");
  
  display.setTextColor(SSD1306_WHITE);
  startScreenText(0, 0, 2, "EWC_Noodle\n");
//  display.clearDisplay();
  startScreenText(0, 0, 1, "");
  instrumentNameOnScreen(0, 15, 0, primaryInstrument);
  // Cycle through the modes to get them printed on the screen
  decrementMenu();
  decrementMenu();
  Serial.print("\t.");

  pcf20.digitalWrite(connectLEDRight, HIGH);
  pcf20.digitalWrite(connectLEDLeft, HIGH);
  pcf21.digitalWrite(controllerLED0, HIGH);
  pcf21.digitalWrite(controllerLED1, HIGH);
  pcf21.digitalWrite(controllerLED2, HIGH);
  pcf21.digitalWrite(controllerLED3, HIGH);
  
  Serial.print(F("\t.\n"));
  Serial.print(F("Startup complete\n"));
}

void loop() {
  readAnalogInputs();
  readIOExpanders();
}
