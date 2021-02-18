#include "q_learner.h"
#include <stdlib.h>
using namespace std;


//https://www.geeksforgeeks.org/multidimensional-arrays-c-cpp/

/*
   Design the serial communication

   int serial_message[5]

   serial_message[0] => operation name
   0 => q_learner.addExperience
   1 => q_learner.decideAction
   2 => q_learner.reset
   3 => generic test

   serial_message[1] => state, type int
   serial_message[2] => action, type int or max-action-size if calling reset
   serial_message[3] => reward, type float
   serial_message[4] => state_prime, type int
*/

// Declare variables

// Message support
static const int MESSAGE_SIZE = 5;
int message[MESSAGE_SIZE];
bool messageWaiting;
const int LOOP_DELAY = 50;

static const int FUNCTION_INDEX = 0;
static const int STATE_INDEX = 1;
static const int ACTION_INDEX = 2;
static const int REWARD_INDEX = 3;
static const int STATE_PRIME_INDEX = 4;

// Within the FUNCTION_INDEX, key to map to correct function request
static const int REQUEST_ADD_EXPERIENCE = 0;
static const int REQUEST_DECIDE_ACTION = 1;
static const int REQUEST_RESET = 2;
static const int REQUEST_TEST = 3;

int state;
int action;
int action_out;
float reward;
int state_prime;
int total_experience;

// Q-learner support
float learning_rate = .1;
float gamma = .99;
QLearner q_learner = QLearner(learning_rate, gamma);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); //start serial port
  // print example: Serial.println("Failed to detect and initialize sensor!");
  // Consume incoming message
  pinMode(LED_BUILTIN, OUTPUT); // initialize digital pin LED_BUILTIN as an output.
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  fillIncoming();

  if (messageWaiting)
  {
        //https://forum.arduino.cc/index.php?topic=396450
        switch (message[FUNCTION_INDEX])
        {
          case REQUEST_ADD_EXPERIENCE:
            // Populate values from message
            state = message[STATE_INDEX];
            action = message[ACTION_INDEX];
            reward = (float)(message[REWARD_INDEX]);
            // Subtract 100 from reward
            reward = reward - 100.0;
            state_prime = message[STATE_PRIME_INDEX];
            q_learner.addExperience(state, action, reward, state_prime);
            break;
          case REQUEST_DECIDE_ACTION:
            // Populate state value from message
            state = message[STATE_INDEX];
            action_out = q_learner.decideAction(state);
            if ((action_out >= 0) || (action_out < q_learner.action_size)) {
              Serial.println(action_out);
            } else {
              Serial.println("getting weird action");
            }
            break;
          case REQUEST_RESET:
            action = message[ACTION_INDEX];
            q_learner.reset(action);
            break;
          case REQUEST_TEST:
            // Obtain reward for particular point in table
            //Serial.write(q_learner.experience_count);
            Serial.println(q_learner.experience_count);
            break;
        }
//    if (message[FUNCTION_INDEX] == REQUEST_TEST) {
//      reward = message[REWARD_INDEX];
//      Serial.println(reward);
//    }
//    test_value = message[0];
//    Serial.println(test_value);
    messageWaiting = false;
  }
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(LOOP_DELAY);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(LOOP_DELAY);
}

void fillIncoming()
{
  // Fill incoming array with Serial.read()
  while (Serial.available() >= MESSAGE_SIZE)
  {
    // fill array with loop
    for (int i = 0; i < MESSAGE_SIZE; i++)
    {
      message[i] = Serial.read();
    }
    messageWaiting = true;
  }
}
