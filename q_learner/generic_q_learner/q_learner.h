class QLearner {

    // Constants
    static const int MIN_EXPERIENCE = 0;
    static const int MAX_EXPERIENCE = 75;
    static const int EXPERIENCE_REPLAY = 50;
    static const int MAX_STATE_SIZE = 25;
    static const int MAX_ACTION_SIZE = 5;

    // Variables
  public:
    float learning_rate;
    float gamma;
    int action_size;  // Use to track the real action space size

    // Initialize q-table
    float q_table[MAX_STATE_SIZE][MAX_ACTION_SIZE] = {0.0};

    // Initialize experience-replay support
    int experience_state[MAX_EXPERIENCE] = {0};
    int experience_state_prime[MAX_EXPERIENCE] = {0};
    float experience_reward[MAX_EXPERIENCE] = {0.0};
    int experience_action[MAX_EXPERIENCE] = {0};

    // Experience count starts at 0
    int experience_count = 0;


    // Standard constructor
    QLearner(float learning_rate, float gamma);
    QLearner(int action_size, float learning_rate, float gamma);
    // Manages experience and calls experience replay for learning if enough experience present
    void addExperience(int state, int action, float reward, int state_prime);
    int decideAction(int state);
    void reset(int action_size);

  private:
    void experienceReplay();



};
