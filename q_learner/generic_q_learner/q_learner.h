class QLearner {

    // Constants
    static const int MAX_EXPERIENCE = 50;
    static const int MAX_STATE_SIZE = 25;  // Roughly 5x5 max board size
    static const int MAX_ACTION_SIZE = 5;

    // Variables
    float learningRate;
    float gamma;
    int actionSize;  // Use to track the real action space size
    int experienceCount = 0;
    
  public:
  
    // Initialize q-table
    float qTable[MAX_STATE_SIZE][MAX_ACTION_SIZE] = {0.0};

    // Initialize experience-replay support
    int experienceState[MAX_EXPERIENCE] = {0};
    int experienceStatePrime[MAX_EXPERIENCE] = {0};
    float experienceReward[MAX_EXPERIENCE] = {0.0};
    int experienceAction[MAX_EXPERIENCE] = {0};

    // Standard constructor
    QLearner(float learningRate, float gamma);
    QLearner(int actionSize, float learningRate, float gamma);

    // Manages experience and calls experience replay for learning if enough experience present
    void addExperience(int state, int action, float reward, int statePrime);  // Store experience for later learning
    int decideAction(int state);  // Reference the Q-table to get the best action, given the state
    void train(int state, int action, float reward, int statePrime);  // Make a single update to the Q-table
    void experienceReplay(int randomIterations); // Randomly loop through the past available experience and update the Q-table
    void reset(int actionSize);  // Reset the Q-table and specify the size of the action-space

    // Get functions
    int getMaxExperience();
    int getMaxStateSize();
    int getMaxActionSize();
    float getLearningRate();
    float getGamma();
    int getActionSize();
    int getExperienceCount();
    
};
