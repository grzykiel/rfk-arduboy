// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <string.h>
#include "macros.h"

//game modes
#define MODE_START                0
#define MODE_MAP                  1
#define MODE_DIALOGUE             2
#define MODE_RESTART              3
#define MODE_FINDKITTEN_ANIM      4
#define MODE_FINDKITTEN_DIALOGUE  5
#define MODE_PLAYAGAIN            6

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
#define KITTEN    256 // arbitrary index greater than NUM_NKI
#define HEART     '\x03'
#define CURSOR    '\x10'

#define YES 1
#define NO  0

#define NUM_NKIS  2

// types
typedef struct {
  int x;
  int y;
} position_t;

typedef struct {
  char character;
  position_t pos;
  char* dialogue;
} nki_t;

// function prototypes
// void display(char* msg);
// position_t screenPos(position_t pos);
 

//globals
const char* intro[] = {
  "robotfindskitten\nBy the illustrious\nLeonard Richardson\n(C) 1997\nWritten especially\nfor the Nerth Pork\nrobotfindskitten\ncontest",
  "Adapted for Arduboy\nby GRZYKIEL",
  "In this game, you\nare robot (#). Your\njob is to find\nkitten. This task\nis complicated by\nthe existence of\nvarious things which\nare not kitten.",
  "Robot must touch\nitems to determine\nif they are kitten\nor not. The game\nends when\nrobotfindskitten.",
  "Alternatively, you\nmay end the game by\nhitting the B\nbutton. See the\ndocumentation for\nmore information.",
  "Press any button to\nstart."
};
int introIndex = 0;
int NUM_INTRO;

const char* dialogues[] = {
  "A signpost saying \"TO KITTEN\". It points in no particular direction.",
  "Seven 1/4\" screws and a piece of plastic.",
  "A third message"
};

int nki_index = -1;

int NUM_DIALOGUES;

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
    case MODE_DIALOGUE:
      modeDialogue();
      break;
    case MODE_FINDKITTEN_ANIM:
      modeFindKittenAnimation();
      break;
    case MODE_PLAYAGAIN:
      modePlayAgain();
      break;
  }
  
  arduboy.display();
}

void modeStart() {
  initialise();
  printn(intro[introIndex]);
  if (arduboy.justPressed(A_BUTTON)) {
    MODE = MODE_MAP;
    return;
  } else if (arduboy.justPressed(B_BUTTON)) {
    if (introIndex < NUM_INTRO-1) {
      introIndex++;
    } 
    else {
      introIndex = 0;
      MODE = MODE_MAP;
      return;
    }
  }
  // arduboy.setCursor(CHAR_WIDTH*2, SCREEN_BOTTOM_PX/2);

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
    
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
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

void modePlayAgain() {
  printn("You found kitten!\n");
  printn("Way to go, robot!\n");
  printn("\n");
  printn("Play again?\n");
  printn("\n");
  printn(" Yes\n No");
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

  NUM_DIALOGUES = sizeof(dialogues)/sizeof(dialogues[0]);
  NUM_INTRO = sizeof(intro)/sizeof(intro[0]);
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
  for (int i=0; i<NUM_NKIS; i++) {
    NKI[i].character = randomCharacter();
    NKI[i].dialogue = dialogues[random(0, NUM_DIALOGUES)];
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

void displayDialogue(char* msg) {
  arduboy.clear();
  arduboy.drawRect(SCREEN_LEFT_PX, SCREEN_TOP_PX, 
                   SCREEN_RIGHT_PX, SCREEN_BOTTOM_PX);
  
  int x=2,y=2;

  int start = 0;
  int end = 19;
  int remaining = strlen(msg);
  while (remaining > 19) {
    while (*(msg+end)!=' ') {
      end--;
    }
    arduboy.setCursor(x,y);
    for (int i=start; i<end; i++) {
      arduboy.print(*(msg+i));
    }
    y+=CHAR_HEIGHT;
    remaining -= (end-start);
    start = end+1;
    end = start+19;
  }
  arduboy.setCursor(x,y);
  for (int i=start; i<strlen(msg); i++) {
     arduboy.print(*(msg+i));
  }
}
  
//recognise newline characters
void printn(char* str) {
  int N = strlen(str);
  for (int i=0; i<N; i++) {
    arduboy.print(*(str+i));
  }
}