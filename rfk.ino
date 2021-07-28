

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

typedef struct {
  int x;
  int y;
} position_t;

void displayDialogue(char* msg);
position_t screenPos(position_t pos);
 


char message[] = "A signpost saying \"TO KITTEN\". It points in no particular direction.";

char msgs[2][100] = {"first", "second"};

position_t roboPos;


Arduboy2 arduboy;

void setup() {
  initialise();
  arduboy.begin();
  arduboy.clear();
  




}

void loop() {
  arduboy.clear();
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
  roboPos = target;
  arduboy.print(roboPos.x);
  arduboy.print(",");
  arduboy.print(roboPos.y);
  position_t screen = screenPos(roboPos);
  arduboy.setCursor(screen.x, screen.y);
  arduboy.print("#");
  arduboy.display();
}

void initialise() {
  roboPos.x=5;
  roboPos.y=5;
}

position_t screenPos(position_t pos) { 
  position_t temp;
  temp.x = pos.x*CHAR_WIDTH;
  temp.y = pos.y*CHAR_HEIGHT;
  return temp;
}


void display(char* msg) {
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
  for (int i=start; i<start+remaining; i++) {
     arduboy.print(*(msg+i));
  }
  
}
