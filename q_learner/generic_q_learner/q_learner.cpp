#include "q_learner.h"
#include "Arduino.h"


QLearner::QLearner(float learningRate, float gamma)
{
  // Initialize without specifying action-size results in using the maximum allowed action size
  // this is only for convenience, the "reset" function should then be used to set the correct action size.
  (*this).actionSize = (*this).MAX_ACTION_SIZE;
  (*this).learningRate = learningRate;
  (*this).gamma = gamma;
}

QLearner::QLearner(int actionSize, float learningRate, float gamma)
{
  // Initialize with action-size, learning-rate, and discount-rate(gamma).
  (*this).actionSize = actionSize;
  (*this).learningRate = learningRate;
  (*this).gamma = gamma;
}

void QLearner::addExperience(int state, int action, float reward, int statePrime)
{

  if (experienceCount >= MAX_EXPERIENCE)
  {
    // If experienceCount is at or over the limit, randomly remove one example before adding
    int i = random(0, experienceCount);
    // Replace experience found at i with latest experience
    experienceState[i] = experienceState[i - 1];
    experienceAction[i] = experienceAction[i - 1];
    experienceReward[i] = experienceReward[i - 1];
    experienceStatePrime[i] = experienceStatePrime[i - 1];
    // Decrement experienceCount
    experienceCount--;
  }

  // Add experience to corresponding arrays
  experienceState[experienceCount] = state;
  experienceAction[experienceCount] = action;
  experienceReward[experienceCount] = reward;
  experienceStatePrime[experienceCount] = statePrime;

  // Increment experience_count
  experienceCount++;
}

int QLearner::decideAction(int state)
{
  // Get max Q-value for state
  float q_value = 0.0;
  float max_q_value = -500.0;
  int best_a = 0;

  for (int a = 0; a < actionSize; a++)
  {
    // Get Q-value for s/action Q(s,a)
    q_value = qTable[state][a];
    if (q_value > max_q_value)
    {
      best_a = a;
      max_q_value = q_value;
    }
  }
  return best_a;
}

void QLearner::train(int state, int action, float reward, int statePrime)
{
  
  // Get original Q-value for state/action -> Q(s,a)
  float q_value = qTable[state][action];
  
  // Get max Q-value for state-prime
  float q_value_prime = 0.0;
  float max_q_value_prime = -500.0;
  for (int k = 0; k < actionSize; k++)
  {
    // Get Q-value for s-prime/action Q(s',a)
    q_value_prime = qTable[statePrime][k];
    if (q_value_prime > max_q_value_prime)
    {
      max_q_value_prime = q_value_prime;
    }
  }
  // Update Q-table with standard q-function
  qTable[state][action] = q_value + (*this).learningRate * (reward + (*this).gamma * max_q_value_prime - q_value);
}

void QLearner::experienceReplay(int randomIterations)
{
  // Randomly select examples and update Q-table
  for (int i = 0; i < randomIterations; i++)
  {
    // Obtain random integer between 0 and experience_count
    int experience_i = random(0, experienceCount);

    // Extract experience tuple for experience_i
    int s = experienceState[experience_i];
    int a = experienceAction[experience_i];
    float r = experienceReward[experience_i];
    int s_prime = experienceStatePrime[experience_i];

    // Call train to update the Q-table
    train(s, a, r, s_prime);
  }
}

void QLearner::reset(int actionSize)
{
  (*this).actionSize = actionSize;
  for (int s = 0; s < MAX_STATE_SIZE; s++)
  {
    for (int a = 0; a < MAX_ACTION_SIZE; a++)
    {
      // Set to default value of zero
      qTable[s][a] = 0.0;
    };
  };
  // Reset experience counter
  experienceCount = 0;
}

int QLearner::getMaxExperience()
{
  return MAX_EXPERIENCE;
}

int QLearner::getMaxStateSize()
{
  return MAX_STATE_SIZE;
}

int QLearner::getMaxActionSize()
{
  return MAX_ACTION_SIZE;
}

float QLearner::getLearningRate()
{
  return learningRate;
}

float QLearner::getGamma()
{
  return gamma;
}

int QLearner::getActionSize()
{
  return actionSize;
}

int QLearner::getExperienceCount()
{
  return experienceCount;
}
