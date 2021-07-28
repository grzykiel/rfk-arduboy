

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

#define ROBOCHAR "#"

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

const char characters[] = "`~1234567890!@$%^&*()[]{}\\|\'\",<.>-_;:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int NUM_CHARS;

const char* dialogues[] = {
  "A signpost saying \"TO KITTEN\". It points in no particular direction.",
  "Seven 1/4\" screws and a piece of plastic."
};

int NUM_DIALOGUES;

position_t roboPos;

nki_t NKI[NUM_NKIS];


int MAP[SCREEN_RIGHT][SCREEN_BOTTOM];

int MODE = 0;

int NKI_INDEX = -1;

Arduboy2 arduboy;

void setup() {
  initialise();
  arduboy.begin();
  arduboy.initRandomSeed();
  arduboy.clear();
  
  
  
  // for (int i=0; i<SCREEN_BOTTOM; i++) {
  //   for (int j=0; j<SCREEN_RIGHT; j++) {
  //     arduboy.print(MAP[j][i]);
  //   }
  //   arduboy.print("\n");
  // }
  
  
  arduboy.display();
}

void loop() {
  arduboy.clear();
  displayNKIs();
  
  if (MODE==0) {
    arduboy.pollButtons();
    position_t target = roboPos;
    if (arduboy.justPressed(UP_BUTTON)) {
      target.y = max(roboPos.y-1, SCREEN_TOP);
    } else if (arduboy.justPressed(DOWN_BUTTON)) {
      target.y = min(roboPos.y+1, SCREEN_BOTTOM);
    } else if (arduboy.justPressed(LEFT_BUTTON)) {
      target.x = max(roboPos.x-1, SCREEN_LEFT);
    } else if (arduboy.justPressed(RIGHT_BUTTON)) {
      target.x = min(roboPos.x+1, SCREEN_RIGHT);
    }
    
    if (!occupied(target)) {
      roboPos = target;
    } else {
      MODE=1;
      NKI_INDEX = MAP[target.x][target.y];
    }
    position_t screen = screenPos(roboPos);
    arduboy.setCursor(screen.x, screen.y);
    arduboy.print(ROBOCHAR);
  } else {
    displayDialogue(NKI[NKI_INDEX].dialogue);
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
  roboPos = randomSpawnPoint();
  while (occupied(roboPos)) {
    roboPos = randomSpawnPoint();
  }
  spawnNKIs();
}

void initialiseMap() {
  for (int i=0; i<SCREEN_RIGHT; i++) {
    for (int j=0; j<SCREEN_BOTTOM; j++) {
      MAP[i][j] = -1;
    }
  }
}

void spawnNKIs() {
  for (int i=0; i<NUM_NKIS; i++) {
    NKI[i].character = characters[random(0,NUM_CHARS)];
    NKI[i].dialogue = dialogues[random(0, NUM_DIALOGUES)];
    position_t spawnPos = randomSpawnPoint();
    while (occupied(spawnPos)) {
      spawnPos = randomSpawnPoint();
    }
    MAP[spawnPos.x][spawnPos.y] = i;
    NKI[i].pos = spawnPos;
  }
}

void displayNKIs() {
  for (int i=0; i<NUM_NKIS; i++) {
    position_t nkiPos = screenPos(NKI[i].pos);
    arduboy.setCursor(nkiPos.x, nkiPos.y);
    arduboy.print(NKI[i].character);
  }
}

position_t randomSpawnPoint() {
  position_t temp;
  temp.x = random(SCREEN_LEFT, SCREEN_RIGHT+1);
  temp.y = random(SCREEN_TOP, SCREEN_BOTTOM+1);
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
