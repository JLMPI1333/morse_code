///////////////////////////////////////////////////////////////////////////////////////
//   Morse Decoder.
//
//   2016-10-30    Jesse Mattern
//
//   Press the button in a series of dashes and dots and see your input turn into the english letter.
//
//
//   This program does the following things.
//
//   * sets up and draws a home screen.
//   * Detect and debounce a button press on a digital pin.
//   * determine if it's a short button press (a dot), or a long button press (a dash).
//   * put that in the top line of the LCD as a - or .
//   * if a pause is detected,  see if the set of dashes and dots are a letter.
//   * if a letter is found, put the letter in the bottom line of the lcd.
//   * if there is a longer pause, completely reset the screen to the begining again.
//
//   NOTE: this assumes an arduino uno, and 16x2 LCD screen.
//         the circuit is similar to the Magic 8 ball example.
//
///////////////////////////////////////////////////////////////////////////////////////

#include <LiquidCrystal.h>

LiquidCrystal lcd (12,11,5,4,3,2);
int nCharPos = 0;
int nDebugPos = 0;
bool starting_screen_displayed;

const int DIGITAL_INPUT_PIN_NUMBER = 6;
int switchState=0;
int prevSwitchState = 0;
unsigned long accumulator = 1;

unsigned long ulWentHigh;
unsigned long ulWentLow;

// A couple of lookups to store the letter, and the mask for that letter.  
#define LOOKUP_TABLE_SIZE 36
unsigned long LOOKUP_BITS[LOOKUP_TABLE_SIZE];
char LOOKUP_LETTER[LOOKUP_TABLE_SIZE];

// Time in milliseconds to redraw the home screen
#define SCREEN_REFRESH_TIME 4000

// Time in milliseconds to detect that you are finished with the series of dashes/dots and we should see if it's a letter.
#define PAUSE_TIME 500

// Time in milliseconds to detect if the button press is a 'short' dot, or a 'long' dash.
#define DASH_TIME 150

#define DOT 0
#define DASH 1

// Enabling this will produce the lookup table in the serial output window.
#define SERIAL_DEBUGGING 1

///////////////////////////////////
// bitme
//    turn a bunch of dashes and dots into our mask value that we will use for lookups.
//    This mask is really just a series of bits,  with a first bit being added automatically.   
//    So  for example :
//    1.) To get the mask for 'a', DOT DASH, This would turn into  101 in binary or 5 in decimal.
//    2.) To get the mask for 'b'. DASH DOT DOT DOT.   This would turn into 10111 or 24 in decimal.
//////////////////////////////////
unsigned long bitme ( int arg1, int arg2 = -1, int arg3 = -1, int arg4 = -1, int arg5 = -1)
{
  unsigned long ulRet = 1;  // automatic bit,  so we dont lose the most significant digit if it's a 0.. starts with a dot.
  ulRet = ulRet << 1;
  ulRet += arg1;
  if(arg2 != -1){
    ulRet = ulRet << 1;
    ulRet += arg2;
  }
  if(arg3 != -1){
    ulRet = ulRet << 1;
    ulRet += arg3;
  }
  if(arg4 != -1){
    ulRet = ulRet << 1;
    ulRet += arg4;
  }
  if(arg5 != -1){
    ulRet = ulRet << 1;
    ulRet += arg5;
  }
  return ulRet;
}

///////////////////////////////////
// draw_starting_screen
//////////////////////////////////
void draw_starting_screen()
{
  nCharPos = 0;
  nDebugPos = 0;
  accumulator = 1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Morse Decoder");
  lcd.setCursor(0,1);
  lcd.print("Ready : ");
  starting_screen_displayed = true;
}

///////////////////////////////////
// register_lookups
//////////////////////////////////
void register_lookups(unsigned long ulMask,char theLetter,int theIndex)
{
  LOOKUP_BITS[theIndex] = ulMask;
  LOOKUP_LETTER[theIndex] = theLetter;
}
///////////////////////////////////
// find_character_by_mask
//////////////////////////////////
char find_character_by_mask(unsigned long ulMask)
{
  for (int i = 0 ; i < LOOKUP_TABLE_SIZE; i++)
  {
    if(LOOKUP_BITS[i] == ulMask)
    {
      return LOOKUP_LETTER[i];
    }
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
// setup
//    1.) setup the Serial communications.
//    2.) setup the LCD parameters
//    3.) declare the digital input for the button.
//    4.) draws the home, or starting screen.
//    5.) Registers two lookup tables with values.  Prints it out if requested.
/////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(DIGITAL_INPUT_PIN_NUMBER,INPUT);
  draw_starting_screen();

  if ( SERIAL_DEBUGGING )
  Serial.println("Serial Debugging enabled.");
  
  unsigned long LETTER_A = bitme(DOT,DASH);
  unsigned long LETTER_B = bitme(DASH,DOT,DOT,DOT);
  unsigned long LETTER_C = bitme(DASH,DOT,DASH,DOT);
  unsigned long LETTER_D = bitme(DASH,DOT,DOT);
  unsigned long LETTER_E = bitme(DOT);
  unsigned long LETTER_F = bitme(DOT,DOT,DASH,DOT);
  unsigned long LETTER_G = bitme(DASH,DASH,DOT);
  unsigned long LETTER_H = bitme(DOT,DOT,DOT,DOT);
  unsigned long LETTER_I = bitme(DOT,DOT);
  unsigned long LETTER_J = bitme(DOT,DASH,DASH,DASH);
  unsigned long LETTER_K = bitme(DASH,DOT,DASH);
  unsigned long LETTER_L = bitme(DOT,DASH,DOT,DOT);
  unsigned long LETTER_M = bitme(DASH,DASH);
  unsigned long LETTER_N = bitme(DASH,DOT);
  unsigned long LETTER_O = bitme(DASH,DASH,DASH);
  unsigned long LETTER_P = bitme(DOT,DASH,DASH,DOT);
  unsigned long LETTER_Q = bitme(DASH,DASH,DOT,DASH);
  unsigned long LETTER_R = bitme(DOT,DASH,DOT);
  unsigned long LETTER_S = bitme(DOT,DOT,DOT);
  unsigned long LETTER_T = bitme(DASH);
  unsigned long LETTER_U = bitme(DOT,DOT,DASH);
  unsigned long LETTER_V = bitme(DOT,DOT,DOT,DASH);
  unsigned long LETTER_W = bitme(DOT,DASH,DASH);
  unsigned long LETTER_X = bitme(DASH,DOT,DOT,DASH);
  unsigned long LETTER_Y = bitme(DASH,DOT,DASH,DASH);
  unsigned long LETTER_Z = bitme(DASH,DASH,DOT,DOT);
  unsigned long LETTER_0 = bitme(DASH,DASH,DASH,DASH,DASH);
  unsigned long LETTER_1 = bitme(DOT,DASH,DASH,DASH,DASH);
  unsigned long LETTER_2 = bitme(DOT,DOT,DASH,DASH,DASH);
  unsigned long LETTER_3 = bitme(DOT,DOT,DOT,DASH,DASH);
  unsigned long LETTER_4 = bitme(DOT,DOT,DOT,DOT,DASH);
  unsigned long LETTER_5 = bitme(DOT,DOT,DOT,DOT,DOT);
  unsigned long LETTER_6 = bitme(DASH,DOT,DOT,DOT,DOT);
  unsigned long LETTER_7 = bitme(DASH,DASH,DOT,DOT,DOT);
  unsigned long LETTER_8 = bitme(DASH,DASH,DASH,DOT,DOT);
  unsigned long LETTER_9 = bitme(DASH,DASH,DASH,DASH,DOT);
  
  register_lookups(LETTER_A,'A',0);
  register_lookups(LETTER_B,'B',1);
  register_lookups(LETTER_C,'C',2);
  register_lookups(LETTER_D,'D',3);
  register_lookups(LETTER_E,'E',4);
  register_lookups(LETTER_F,'F',5);
  register_lookups(LETTER_G,'G',6);
  register_lookups(LETTER_H,'H',7);
  register_lookups(LETTER_I,'I',8);
  register_lookups(LETTER_J,'J',9);
  register_lookups(LETTER_K,'K',10);
  register_lookups(LETTER_L,'L',11);
  register_lookups(LETTER_M,'M',12);
  register_lookups(LETTER_N,'N',13);
  register_lookups(LETTER_O,'O',14);
  register_lookups(LETTER_P,'P',15);
  register_lookups(LETTER_Q,'Q',16);
  register_lookups(LETTER_R,'R',17);
  register_lookups(LETTER_S,'S',18);
  register_lookups(LETTER_T,'T',19);
  register_lookups(LETTER_U,'U',20);
  register_lookups(LETTER_V,'V',21);
  register_lookups(LETTER_W,'W',22);
  register_lookups(LETTER_X,'X',23);
  register_lookups(LETTER_Y,'Y',24);
  register_lookups(LETTER_Z,'Z',25);
  register_lookups(LETTER_0,'0',26); 
  register_lookups(LETTER_1,'1',27); 
  register_lookups(LETTER_2,'2',28); 
  register_lookups(LETTER_3,'3',29); 
  register_lookups(LETTER_4,'4',30); 
  register_lookups(LETTER_5,'5',31); 
  register_lookups(LETTER_6,'6',32); 
  register_lookups(LETTER_7,'7',33); 
  register_lookups(LETTER_8,'8',34); 
  register_lookups(LETTER_9,'9',35);
 
 if ( SERIAL_DEBUGGING )
 {
  for (int i = 0; i < LOOKUP_TABLE_SIZE; i++)
  {
    Serial.print(i); 
    Serial.print(". ");
    Serial.print(LOOKUP_LETTER[i]);
    Serial.print(" - ");
    Serial.println(LOOKUP_BITS[i]);
  }
 } 
/*
Letter	Morse
A	.-
B	-...
C	-.-.
D	-..
E	.
F	..-.
G	--.
H	....
I	..
J	.---
K	-.-
L	.-..
M	--
N	-.
O	---
P	.--.
Q	--.-
R	.-.
S	...
T	-
U	..-
V	...-
W	.--
X	-..-
Y	-.--
Z	--..

0	-----
1	.----
2	..---
3	...--
4	....-
5	.....
6	-....
7	--...
8	---..
9	----.
*/
}

///////////////////////////////////
// reset_dash_dot_on_lcd
//////////////////////////////////
void reset_dash_dot_on_lcd()
{
    lcd.setCursor(0,0);
    lcd.print("                ");
    nDebugPos = 0;
    accumulator = 1;
}

///////////////////////////////////
// print_to_screen_top_line
//////////////////////////////////
void print_to_screen_top_line(int state)
{
  if(starting_screen_displayed)
     lcd.clear();
  starting_screen_displayed = false;
  print_dash_dot(state);
  if(nDebugPos>5) {
    // This set of input is not valid
    reset_dash_dot_on_lcd();
    print_a_letter('?');
  }
}

///////////////////////////////////
// find_char_after_break
//////////////////////////////////
void find_char_after_break()
{
  char c = find_character_by_mask(accumulator);
  if(c == NULL){ c='?'; }
  reset_dash_dot_on_lcd();
  print_a_letter(c);
}

///////////////////////////////////
// print_dash_dot
//////////////////////////////////
void print_dash_dot(int state)
{
  lcd.setCursor(nDebugPos,0);
  lcd.print( state == DOT ? '.' : '-');
  nDebugPos++;
}
///////////////////////////////////
// print_a_letter
//////////////////////////////////
void print_a_letter(char c)
{
  lcd.setCursor(nCharPos,1);
  lcd.print(c);
  nCharPos++;
}

#define DEBOUNCE_COUNT 10

///////////////////////////////////
// debounce_state_change
//////////////////////////////////
int debounce_state_change()
{
  int counter = 0;
  int this_read = LOW;
  int last_read = LOW;
  
  while(true)
  {
    this_read = digitalRead(DIGITAL_INPUT_PIN_NUMBER);
    if(this_read == last_read)
      counter++;
    else
      counter--;
    last_read = this_read;
    if(counter >= DEBOUNCE_COUNT)
       return this_read;
  }
}


///////////////////////////////////
// process_dash_dot
//////////////////////////////////
void process_dash_dot(int dash_dot) 
{
  if (accumulator > 1024) accumulator = 1; // something went wrong, recover.
  accumulator = accumulator << 1; // prepare mask for another button that was pressed
  accumulator += dash_dot;  // add a 0 or 1 to the accumulator mask
  print_to_screen_top_line(dash_dot);
}

///////////////////////////////////////////////////////////////////////////////////////
// Main Loop
///////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  int switchState = debounce_state_change();
  if ( prevSwitchState != switchState) 
  {
    if (switchState == LOW)
    {
      ulWentLow = millis();
      if ( (ulWentLow - ulWentHigh ) > DASH_TIME )
      {
        process_dash_dot(DASH);
      }
      else
      {
        process_dash_dot(DOT);
      }
    }
    else
    {
        ulWentHigh = millis();
    }
    prevSwitchState = switchState;
  }
  
  // Do we have a break (a complete set of dashes / dots)
  if ( (switchState == LOW ) &&
       (accumulator > 1 ) &&
       (millis() - ulWentLow)> PAUSE_TIME)
   {
     find_char_after_break();
   }
       
  // The user hasn't clicked in a while... go back to the starting screen
  if ( !starting_screen_displayed &&
       (switchState == LOW) && 
       ((millis() - ulWentLow)> SCREEN_REFRESH_TIME))
  {
    draw_starting_screen();
  }
}
