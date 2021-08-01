// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <string.h>
#include "macros.h"
#include "text.h"

//game modes
#define MODE_START                0
#define MODE_MAP                  1
#define MODE_DIALOGUE             2
#define MODE_RESTART              3
#define MODE_FINDKITTEN_ANIM      4
#define MODE_FINDKITTEN_DIALOGUE  5
#define MODE_PLAYAGAIN            6
#define MODE_TEST                 7

#define ANY_BUTTON A_BUTTON | B_BUTTON | UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON

//constants
#define SCREEN_LEFT   0
#define SCREEN_RIGHT  20
#define SCREEN_TOP    0
#define SCREEN_BOTTOM 7

#define SCREEN_LEFT_PX    0
#define SCREEN_RIGHT_PX   128
#define SCREEN_TOP_PX     0
#define SCREEN_BOTTOM_PX  64

#define CHAR_WIDTH  6
#define CHAR_HEIGHT 8

#define FRAME_RATE 60

#define MAX_DIALOGUE_SIZE 140 //screen size in characters

#define ROBOCHAR  '#'
#define KITTEN    255 // arbitrary index greater than NUM_NKI
#define HEART     '\x03'
#define CURSOR    '\x10'

#define YES 1
#define NO  0

#define NUM_NKIS      14
#define NUM_DIALOGUES 199
#define NUM_INTRO     5

// types
typedef struct {
  uint8_t x;
  uint8_t y;
} position_t;

typedef struct {
  char character;
  position_t pos;
  uint8_t dialogue;
} nki_t;

//globals
uint8_t introIndex = 0;
uint8_t nki_index = -1;

nki_t NKI[NUM_NKIS];

nki_t robot;
nki_t kitten;

int MAP[SCREEN_RIGHT+1][SCREEN_BOTTOM+1];

byte MODE = MODE_START;

byte animationFrames = 0;

byte cursorSelection = YES;

Arduboy2 arduboy;


void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  arduboy.initRandomSeed();
}

void loop() {
  if(!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();
  
  switch (MODE) {
    case MODE_START:
      modeStart();
      break;
    case MODE_MAP:
      modeMap();
      break;
    case MODE_RESTART:
      modeRestart();
      break;
    case MODE_DIALOGUE:
      modeDialogue();
      break;
    case MODE_FINDKITTEN_ANIM:
      modeFindKittenAnimation();
      break;
    case MODE_PLAYAGAIN:
      modePlayAgain();
      break;
    case MODE_TEST:
      modeTest();
      break;
  }
  arduboy.display();
}


void modeTest() {

}

void modeStart() {
  initialise();
  char message[MAX_DIALOGUE_SIZE];
  strcpy_P(message, (PGM_P)pgm_read_word(&(intro[introIndex])));
  arduboy.print(message);
  if (introIndex < NUM_INTRO) {
    if (arduboy.justPressed(A_BUTTON)) {
      introIndex = 0;
      MODE = MODE_MAP;
      return;
    } else if (arduboy.justPressed(B_BUTTON | DOWN_BUTTON | RIGHT_BUTTON)) {
      introIndex++;
    } else if (arduboy.justPressed(UP_BUTTON | LEFT_BUTTON)) {
      introIndex = max(0, introIndex-1);
    }
  } else {
    if (arduboy.justPressed(ANY_BUTTON)) {
      introIndex = 0;
      MODE = MODE_MAP;
      return;
    }
  }
}

void modeMap() {
    position_t target = robot.pos;
    if (arduboy.justPressed(UP_BUTTON)) {
      target.y = max(robot.pos.y-1, SCREEN_TOP);
    } else if (arduboy.justPressed(DOWN_BUTTON)) {
      target.y = min(robot.pos.y+1, SCREEN_BOTTOM);
    } else if (arduboy.justPressed(LEFT_BUTTON)) {
      target.x = max(robot.pos.x-1, SCREEN_LEFT);
    } else if (arduboy.justPressed(RIGHT_BUTTON)) {
      target.x = min(robot.pos.x+1, SCREEN_RIGHT);
    } else if (arduboy.justPressed(A_BUTTON)) {
      MODE=MODE_RESTART;
    }
    
    if (!occupied(target)) {
      robot.pos = target;
    } else {
      nki_index = MAP[target.x][target.y];
      if (nki_index == KITTEN) {
        MODE=MODE_FINDKITTEN_ANIM;
      } else {
        MODE=MODE_DIALOGUE;
      }
    }
    displayNKI(robot);
    displayNKI(kitten);
    displayAllNKIs();

}

void modeDialogue() {
  displayDialogue(NKI[nki_index].dialogue);
    
  if (arduboy.justPressed(ANY_BUTTON)) {
    MODE = MODE_MAP;
  }
}

void modeFindKittenAnimation() {
  if (animationFrames==0) {
      robot.pos.x = SCREEN_LEFT+6;
      robot.pos.y = 4;
      kitten.pos.x = SCREEN_RIGHT-5;
      kitten.pos.y = 4;
  }
  if (arduboy.everyXFrames(60)) {
    if (animationFrames < 4) {
        robot.pos.x++;
        kitten.pos.x--;
    }
    if (animationFrames<6) animationFrames++;
  }
  
  if (animationFrames == 5) {
    position_t heartPos = robot.pos;
    heartPos.y = robot.pos.y-1;
    heartPos = screenPos(heartPos);
    arduboy.setCursor(heartPos.x, heartPos.y);
    arduboy.print(HEART);
    arduboy.print(HEART);  
  } else if (animationFrames==6) {
    MODE = MODE_PLAYAGAIN;
  }
    displayNKI(robot);
    displayNKI(kitten);
}

void modeRestart() {
  arduboy.print(F("Restart?\n"));
  arduboy.print(F("\n"));
  arduboy.print(F(" Yes\n No"));
  if (cursorSelection == YES) {
    arduboy.setCursor(SCREEN_LEFT, 16);
  } else {
    arduboy.setCursor(SCREEN_LEFT, 24);
  }
  arduboy.print(CURSOR);
  if (arduboy.justPressed(DOWN_BUTTON)) {
    cursorSelection = NO;
  } else if (arduboy.justPressed(UP_BUTTON)) {
    cursorSelection = YES;
  } else if (arduboy.justPressed(B_BUTTON)) {
    if (cursorSelection == YES) {
      initialise();
      MODE = MODE_MAP;
    } else {
      MODE = MODE_MAP;
    }
  } else if (arduboy.justPressed(A_BUTTON)) {
    MODE = MODE_MAP;
  }
}

void modePlayAgain() {
  arduboy.print(F("You found kitten!\n"));
  arduboy.print(F("Way to go, robot!\n"));
  arduboy.print(F("\n"));
  arduboy.print(F("Play again?\n"));
  arduboy.print(F("\n"));
  arduboy.print(F(" Yes\n No"));
  if (cursorSelection == YES) {
    arduboy.setCursor(SCREEN_LEFT, 40);
  } else {
    arduboy.setCursor(SCREEN_LEFT, 48);
  }
  arduboy.print(CURSOR);
  if (arduboy.justPressed(DOWN_BUTTON)) {
    cursorSelection = NO;
  } else if (arduboy.justPressed(UP_BUTTON)) {
    cursorSelection = YES;
  }
  if (arduboy.justPressed(B_BUTTON)) {
    if (cursorSelection == YES) {
      initialise();
      MODE = MODE_MAP;
    } else {
      MODE = MODE_START;
    }
  }
}

void initialise() {
  animationFrames = 0;
  initialiseMap();  
  //initialise NKIs
  spawnNKIs();  
  //initialise robot
  robot.character = ROBOCHAR;
  robot.pos = unoccupiedPosition();
  //initialise kitten
  kitten.character = randomCharacter();
  kitten.pos = unoccupiedPosition();
  MAP[kitten.pos.x][kitten.pos.y] = KITTEN;
  cursorSelection = YES;
}

void initialiseMap() {
  for (int i=0; i<=SCREEN_RIGHT; i++) {
    for (int j=0; j<=SCREEN_BOTTOM; j++) {
      MAP[i][j] = -1;
    }
  }
}

void spawnNKIs() {
  for (uint8_t i=0; i<NUM_NKIS; i++) {
    NKI[i].character = randomCharacter();
    NKI[i].dialogue = random(0, NUM_DIALOGUES);
    position_t spawnPos = unoccupiedPosition();
    MAP[spawnPos.x][spawnPos.y] = i;
    NKI[i].pos = spawnPos;
  }
}

void displayAllNKIs() {
  for (int i=0; i<NUM_NKIS; i++) {
    displayNKI(NKI[i]);
  }
}

void displayNKI(nki_t nki) {
  position_t nkiPos = screenPos(nki.pos);
  arduboy.setCursor(nkiPos.x, nkiPos.y);
  arduboy.print(nki.character);
}

position_t randomPosition() {
  position_t temp;
  temp.x = random(SCREEN_LEFT, SCREEN_RIGHT+1);
  temp.y = random(SCREEN_TOP, SCREEN_BOTTOM+1);
  return temp;
}

position_t unoccupiedPosition() {
  position_t temp = randomPosition();
  while (occupied(temp)) {
    temp = randomPosition();
  }
  return temp;
}

position_t screenPos(position_t pos) { 
  position_t temp;
  temp.x = pos.x*CHAR_WIDTH;
  temp.y = pos.y*CHAR_HEIGHT;
  return temp;
}

bool occupied(position_t pos) {
  return (MAP[pos.x][pos.y] > -1);
}

char randomCharacter() {
  int ascii = random(33, 256);
  while (ascii == 35 || (ascii > 126 && ascii < 161)) {
    ascii = random(33, 256);
  }
  return (char) ascii;
}

void displayDialogue(uint8_t d) {
  char message[MAX_DIALOGUE_SIZE];
  strcpy_P(message, (PGM_P)pgm_read_word(&(dialogues[d])));
  int x=0,y=0;

  int start = 0;
  int end = SCREEN_RIGHT+1;
  int length = strlen(message);
  int remaining = length;
  while (remaining > SCREEN_RIGHT+1) {
    while (message[end]!=' ') {
      end--;
    }
    arduboy.setCursor(x,y);
    for (int i=start; i<end; i++) {
      arduboy.print(message[i]);
    }
    y+=CHAR_HEIGHT;
    remaining -= (end-start);
    start = end+1;
    end = start+SCREEN_RIGHT+1;
  }
  arduboy.setCursor(x,y);
  for (int i=start; i<length; i++) {
     arduboy.print(message[i]);
  }
}

