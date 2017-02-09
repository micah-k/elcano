#include <Settings.h>
#include <Servo.h>
Servo STEER_SERVO;

#define INPUT_LOW 1000
#define INPUT_HIGH 2000
#define LEFTS_LOW 435
#define LEFTS_HIGH 527
#define RIGHTS_LOW 536
#define RIGHTS_HIGH 637

int inpNew, input;
double leftS, rightS, leftgoal, rightgoal;
const double maxstep = 5.3158;


void setup() {
  Serial.begin(9600);
  STEER_SERVO.attach(STEER_OUT_PIN);
}

void loop() {
  inpNew = STEER_SERVO.readMicroseconds();
  if(input != inpNew)
  {
    input = inpNew > INPUT_HIGH ? INPUT_HIGH : inpNew;
    input = input < INPUT_LOW ? INPUT_LOW : input;    
  
    leftgoal = map(input, INPUT_LOW, INPUT_HIGH, LEFTS_LOW, LEFTS_HIGH);
    rightgoal = map(input, INPUT_LOW, INPUT_HIGH, RIGHTS_LOW, RIGHTS_HIGH);
  }

  leftS += leftS < leftgoal ? min(leftgoal - leftS, maxstep) : -min(leftS - leftgoal, maxstep);
  rightS += rightS < rightgoal ? min(rightgoal - rightS, maxstep) : -min(rightS - rightgoal, maxstep);

  
}
