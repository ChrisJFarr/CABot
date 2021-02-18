#include "q_learner.h"
#include "Arduino.h"


QLearner::QLearner(float learning_rate, float gamma) {

  (*this).action_size = MAX_ACTION_SIZE;
  (*this).learning_rate = learning_rate;
  (*this).gamma = gamma;

}

QLearner::QLearner(int action_size, float learning_rate, float gamma) {

  (*this).action_size = action_size;
  (*this).learning_rate = learning_rate;
  (*this).gamma = gamma;

}

void QLearner::addExperience(int state, int action, float reward, int state_prime) {
  // Add experience to corresponding arrays
  experience_state[experience_count] = state;
  experience_action[experience_count] = action;
  experience_reward[experience_count] = reward;
  experience_state_prime[experience_count] = state_prime;

  // Increment experience_count
  experience_count++;

  // Call experienceReplay for learning
  experienceReplay();

}

int QLearner::decideAction(int state) {

  // Get max Q-value for state
  float q_value = 0.0;
  float max_q_value = -500.0;
  int best_a = 0;

  for (int a = 0; a < action_size; a++) {
    // Get Q-value for s/action Q(s,a)
    q_value = q_table[state][a];
    if (q_value > max_q_value) {
      best_a = a;
      max_q_value = q_value;
    }
  }
  return best_a;
}

void QLearner::experienceReplay() {

  // If experience_count >= MIN_EXPERIENCE
  if (experience_count >= MIN_EXPERIENCE) {

    // Randomly select examples and update Q-table
    for (int i = 0; i < EXPERIENCE_REPLAY; i++) {

      // Obtain random integer between 0 and experience_count
      int experience_i = random(0, experience_count);

      // Extract experience tuple for experience_i
      int s = experience_state[experience_i];
      int a = experience_action[experience_i];
      float r = experience_reward[experience_i];
      int s_prime = experience_state_prime[experience_i];

      // Get original Q-value for state/action -> Q(s,a)
      float q_value = q_table[s][a];

      // Get max Q-value for state-prime
      float q_value_prime = 0.0;
      float max_q_value_prime = -500.0;
      for (int k = 0; k < action_size; k++)
      {
        // Get Q-value for s-prime/action Q(s',a)
        q_value_prime = q_table[s_prime][k];
        if (q_value_prime > max_q_value_prime)
        {
          max_q_value_prime = q_value_prime;
        }
      }

      // Update Q-table with standard q-function
      q_table[s][a] = q_value + learning_rate * (r + gamma * max_q_value_prime - q_value);

    }
    if (experience_count >= MAX_EXPERIENCE)
    {
      // randomly remove one example and replace with experience at top, decrement experience_count
      // Obtain random integer between 0 and experience_count
      int experience_i = random(0, experience_count);
      // Replace experience found at i with last experience
      experience_state[experience_i] = experience_state[experience_count - 1];
      experience_action[experience_i] = experience_action[experience_count - 1];
      experience_reward[experience_i] = experience_reward[experience_count - 1];
      experience_state_prime[experience_i] = experience_state_prime[experience_count - 1];
      // Decrement experience_count
      experience_count--;
    }
  }
}


void QLearner::reset(int action_size) {
  (*this).action_size = action_size;
  for (int s = 0; s < MAX_STATE_SIZE; s++) {
    for (int a = 0; a < action_size; a++) {
      q_table[s][a] = 0.0;
    };
  };
  // Reset experience counter, no need to update values
  experience_count = 0;
}
