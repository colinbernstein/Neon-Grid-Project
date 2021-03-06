/**
   Author: Colin Bernstein
   Title: Handheld 6x6 Multiplexed Neon Bulb Drawing Game
*/


//Define GPIO pins and instance variables
byte x, y;
const byte cathode[] = {2, 3, 4, 5, 6, 7}, anode[] = {8, 9, 10, 11, 12, 13};
const int blinkRate = 300, tickRate = 1000;
unsigned long lastBlink, lastTick;
boolean ticking, blinking, holding = false, grid[6][6];

//Initizalize hard-wired inputs and outputs
//Zero the grid
void setup() {
  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
  for (byte i = 14; i <= 19; i++)
    pinMode(i, INPUT_PULLUP);
  for (byte a = 0; a < 6; a++)
    for (byte b = 0; b < 6; b++)
      grid[a][b] = a % b == 1;
}

//Constantly multiplex 
//Cycle blinking for currently 
void loop() {
  multPlex();
  if (!ticking && millis() - lastBlink >= blinkRate) {
    lastBlink = millis();
    blinking = !blinking;
  }
  checkButtons();
}

//Check for button presses
//Adjust the position of the cursor accordingly
//If a button is pressed, a button is now being held
void checkButtons() {
  if (!holding && !digitalRead(14)) { //toggle ticking
    holding = true;
    nextGeneration();
  }
  else if (!holding && !ticking && !digitalRead(15)) { //invert x,y
    holding = true;
    grid[y][x] = !grid[y][x];
  }
  else if (!holding && !digitalRead(16)) { //R
    holding = true;
    if (inBounds(x + 1, y))
      x++;
  }
  else if (!holding && !digitalRead(17)) { //U
    holding = true;
    if (inBounds(x , y - 1))
      y--;
  }
  else if (!holding && !digitalRead(18)) { //L
    holding = true;
    if (inBounds(x - 1, y))
      x--;
  }
  else if (!holding && !digitalRead(19)) { //D
    holding = true;
    if (inBounds(x, y + 1))
      y++;
  }
  if (digitalRead(14) && digitalRead(15) && digitalRead(16) && digitalRead(17) && digitalRead(18) && digitalRead(19))
    holding = false;
}

//Multiplex through the columns by flashing a column at a time and outputting the six row elements each time
void multPlex() {
  for (byte col = 0; col < 6; col++) {
    for (byte row = 0; row < 6; row++) {
      digitalWrite(anode[row], grid[row][col] ? HIGH : LOW);
      if (col == x && row == y) digitalWrite(anode[row], blinking ? HIGH : LOW);
    }
    digitalWrite(cathode[col], HIGH);
    delay(1);
    digitalWrite(cathode[col], LOW);
  }
}

boolean inBounds(byte i, byte j) {
  return i >= 0 && j >= 0 && i < 6 && j < 6;
}

//Find the next generation of the current configuration of the grid using a Conway's Game of Life automata
void nextGenerationConway() {
  boolean next[6][6];
  for (byte i = 0; i < 6; i++)
    for (byte j = 0; j < 6; j++) {
      byte n = neighbors(i, j);
      if (grid[i][j] && n < 2)
        next[i][j] = false;
      else if (grid[i][j] && n > 3)
        next[i][j] = false;
      else if (!grid[i][j] && n == 3)
        next[i][j] = true;
      else
        next[i][j] = grid[i][j];
    }
  for (byte i = 0; i < 6; i++)
    for (byte j = 0; j < 6; j++)
      grid[i][j] = next[i][j];
}

//Find a nearest-neighbors favored style next generation 
void nextGeneration() {
  boolean next[6][6];
  for (byte i = 0; i < 6; i++)
    for (byte j = 0; j < 6; j++) {
      byte n = neighbors(i, j);
      if (grid[i][j] && n == 2)
        next[i][j] = false;
      else if (grid[i][j] && n > 5)
        next[i][j] = false;
      else if (!grid[i][j] && (n == 1 || n == 3))
        next[i][j] = true;
      else
        next[i][j] = grid[i][j];
    }
  for (byte i = 0; i < 6; i++)
    for (byte j = 0; j < 6; j++)
      grid[i][j] = next[i][j];
}

//Return the number of neighbors to a given point
byte neighbors(byte i, byte j) {
  byte ret = 0;
  byte dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
  byte dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
  for (byte a = 0; a < 8; a++)
    if (inBounds(i + dx[a], j + dy[a]))
      if (grid[i + dx[a]][j + dy[a]])
        ret++;
  return ret;
}

/*void blank() {
  for (byte col = 0; col < 6; col++)
    digitalWrite(cathode[col], LOW);
  }*/
