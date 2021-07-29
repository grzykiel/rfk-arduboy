

// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <string.h>

//macros
#define max(x, y) x>y?x:y
#define min(x, y) x<y?x:y

//constants
#define SCREEN_LEFT 0
#define SCREEN_RIGHT 20
#define SCREEN_TOP 0
#define SCREEN_BOTTOM 7

#define SCREEN_LEFT_PX 0
#define SCREEN_RIGHT_PX 128
#define SCREEN_TOP_PX 0
#define SCREEN_BOTTOM_PX 64

#define CHAR_WIDTH 6;
#define CHAR_HEIGHT 8;

#define MAX_DIALOGUE_SIZE 140 //screen size in characters

#define ROBOCHAR '#'
#define KITTEN 256 // arbitrary index greater than NUM_NKI

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
void display(char* msg);
position_t screenPos(position_t pos);
 

//globals
const int NUM_NKIS = 5;

char characters[] = "`~1234567890!@$%^&*()[]{}\\|\'\",<.>-_;:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int NUM_CHARS;

const char* dialogues[] = {
  "A signpost saying \"TO KITTEN\". It points in no particular direction.",
  "Seven 1/4\" screws and a piece of plastic.",
  "A third message"
};

char* kittenDialogue = "You found kitten!";

int NUM_DIALOGUES;

nki_t NKI[NUM_NKIS];

nki_t robot;
nki_t kitten;


int MAP[SCREEN_RIGHT+1][SCREEN_BOTTOM+1];

int MODE = 0;

int NKI_INDEX = -1;

Arduboy2 arduboy;

void setup() {
  initialise();
  arduboy.begin();
  arduboy.initRandomSeed();
  arduboy.clear();
  
  
  arduboy.display();
}

void loop() {
  arduboy.clear();
  displayAllNKIs();
  displayNKI(kitten);
  
  if (MODE==0) {
    arduboy.pollButtons();
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
      MODE=1;
      NKI_INDEX = MAP[target.x][target.y];
    }
    position_t screen = screenPos(robot.pos);
    arduboy.setCursor(screen.x, screen.y);
    arduboy.print(robot.character);
  } else {
    if (NKI_INDEX == KITTEN) {
      displayDialogue(kitten.dialogue);
    } else {
      displayDialogue(NKI[NKI_INDEX].dialogue);
    }
    arduboy.pollButtons();
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
      MODE = 0;
    }
  }
  

  arduboy.display();
}

void initialise() {

  NUM_CHARS = strlen(characters);
  NUM_DIALOGUES = sizeof(dialogues)/sizeof(dialogues[0]);
  initialiseMap();  
  //initialise NKIs
  spawnNKIs();  
  //initialise robot
  robot.character = ROBOCHAR;
  robot.pos = unoccupiedPosition();
  //initialise kitten
  kitten.character = random(0, NUM_CHARS);
  kitten.pos = unoccupiedPosition();
  MAP[kitten.pos.x][kitten.pos.y] = KITTEN;
  kitten.dialogue = kittenDialogue;

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
    NKI[i].character = characters[random(0,NUM_CHARS)];
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
