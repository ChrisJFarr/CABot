"""

Build a model of mbot's environment and connect to an arduino to train the q-learner.
In this case the model is in python and only for simulation
The q-learner is on the arduino to test the arduino q-learner implementation




"""

import numpy as np
import random
import serial
import struct


class World:
    ROBOT = "R"
    WALL = "#"
    GOAL = "G"

    def __init__(self, grid_height=5, grid_width=5, n_obstacles=10,
                 robot_starting_loc=(0, 0), goal_loc=None, seed=None):
        # Randomly set seed from pool known to produce solveable mazes
        if seed is None:
            seed_list = [1, 2, 6, 7, 10, 12, 14, 19, 21]
            random.seed(random.choice(seed_list))
        else:
            random.seed(seed)

        self.action_space = 4  # up, down, left, right
        self.n_obstacles = n_obstacles
        if goal_loc is None:
            self.goal_loc = (grid_height - 1, grid_width - 1)
        self.grid_height = grid_height
        self.grid_width = grid_width
        self.grid_world = np.array(["-"] * grid_height * grid_width).reshape((grid_height, grid_width))
        self.robot_loc = robot_starting_loc
        self.prior_robot_loc = None
        self.collision = False  # Update to true is move takes robot to the wall
        self.active = True
        self.add_world_objects()
        self.total_points = 0.0

    def add_world_objects(self):
        obstacle_list = list()
        for i in range(self.n_obstacles):
            while True:
                new_point = (random.randint(a=0, b=self.grid_height - 1), random.randint(a=0, b=self.grid_width - 1))
                if new_point in obstacle_list or new_point == self.robot_loc or new_point == self.goal_loc:
                    continue
                else:
                    break
            obstacle_list.append(new_point)
        for obstacle in obstacle_list:
            x, y = obstacle
            self.grid_world[x][y] = World.WALL
        # Add goal
        self.grid_world[self.goal_loc[0]][self.goal_loc[1]] = World.GOAL
        # Add robot
        self.grid_world[self.robot_loc[0]][self.robot_loc[1]] = World.ROBOT
        return

    def __repr__(self):
        return str(self.grid_world)

    def step(self, action, debug=True):
        # Takes step and returns reward for taking step
        # In this world the robot can drive into a wall but not off the map
        # One of "up" "down" "left" "right
        self.robot_loc: tuple

        if action == 0:  # "up":
            update = (-1, 0)
            if debug:
                print("up")
        elif action == 1:  # "down":
            update = (1, 0)
            if debug:
                print("down")
        elif action == 2:  # "left":
            update = (0, -1)
            if debug:
                print("left")
        elif action == 3:  # "right":
            update = (0, 1)
            if debug:
                print("right")
        else:
            raise ValueError("direction must be one of 'up', 'down', 'left', 'right'. Received %s" % str(action))

        # Store current location before making any changes
        self.prior_robot_loc = self.robot_loc

        # Compute next location
        new_x = self.robot_loc[0] + update[0]
        new_y = self.robot_loc[1] + update[1]

        # Check for boundary collision
        x_collision = (new_x < 0) | (new_x >= len(self.grid_world[0]))
        y_collision = (new_y < 0) | (new_y >= len(self.grid_world[1]))
        boundary_collision = x_collision | y_collision

        # Check for obstacle collision
        if not boundary_collision:
            obstacle_collision = self.grid_world[new_x][new_y] == World.WALL
        else:
            obstacle_collision = False

        # Check for goal
        if not boundary_collision:
            goal_reached = self.grid_world[new_x][new_y] == World.GOAL
        else:
            goal_reached = False

        # If there are no collisions, update world and return true
        if boundary_collision or obstacle_collision:
            self.active = True
            self.collision = True
            new_points = -100
            self.total_points += new_points
            return self.prior_robot_loc, action, new_points, self.robot_loc
        else:
            # Update robot loc
            self.robot_loc = (new_x, new_y)
            self.update_world(self.grid_world)
            if goal_reached:
                # Goal has been reached, end of episode
                self.active = False
                new_points = 100
                self.total_points += new_points
                return self.prior_robot_loc, action, new_points, self.robot_loc
            else:
                # Move succeeds, continuing on with episode
                new_points = 0
                self.total_points += new_points
                return self.prior_robot_loc, action, new_points, self.robot_loc

    def update_world(self, world):
        world[self.prior_robot_loc[0]][self.prior_robot_loc[1]] = "-"
        world[self.robot_loc[0]][self.robot_loc[1]] = World.ROBOT
        return world


# Arduino info
"""
   int serial_message[5]

   serial_message[0] => operation name
   0 => q_learner.addExperience
   1 => q_learner.decideAction
   2 => q_learner.reset

   serial_message[1] => state, type int
   serial_message[2] => action, type int
   serial_message[3] => reward, type float
   serial_message[4] => state_prime, type int
"""

# Simulation example
from time import sleep, time
import math


# https://forum.arduino.cc/index.php?topic=396450
# Testing sending signed int or float
# bit_str = ">BBbBB"
# struct.pack(bit_str, -128)
# arduino.write(struct.pack(bit_str, 1))
# output = arduino.readline()
# output = output.decode()
# output = output.rstrip()
# output = math.floor(float(output))
# output = output - 256
# print(output)
# print(math.floor(float(output)))
# arduino.close()
def add_experience(state, action, reward, state_prime):
    global arduino, bit_str
    function_index = 0
    arduino.write(struct.pack(bit_str, function_index, state, action, reward, state_prime))
    # str_msg = output.decode()
    # str_msg = str_msg.rstrip()
    return


def get_action(state=0):
    global arduino, bit_str
    function_index = 1
    arduino.write(struct.pack(bit_str, function_index, state, 0, 0, 0))
    output = arduino.readline()
    output = output.decode()
    output = output.rstrip()
    if output.isdigit():
        action = int(output)
    else:
        raise Exception(output)
    return action


def reset(action_size=4):
    global arduino, bit_str
    function_index = 2
    arduino.write(struct.pack(bit_str, function_index, 0, action_size, 0, 0))


def test():
    global arduino, bit_str
    function_index = 3  # Test index
    arduino.write(struct.pack(bit_str, function_index, 0, 0, 0, 0))
    output = arduino.readline()
    str_msg = output.decode()
    str_msg = str_msg.rstrip()
    return int(str_msg)

# arduino.readline()
# check = test()
# arduino.flush()
# reset()
# get_action(10)


# Commands for arduino:
# Reset q-table
# Add experience
# Get action
# Automatic: experience replay/dyna, keep 100 random examples and replays them

# Tune parameters to allow for fastest possible training
# epsilon-greedy
arduino = serial.Serial('COM4', 9600, timeout=1)
bit_str = ">BBBBB"
# arduino.close()
reset()
test()
epsilon = .0
min_epsilon = .50
epsilon_decay = .01

"""
Training q-learner through simulation
"""

# Get it done in one episode
simulation_world = World(seed=10)
random.seed(time())
last_shaping_adjustment = 0
shaping_adjustment = 0
# Exploration bonus
# If the robot has not been to a spot before it gets a bonus
exploration_bonus = np.ones(simulation_world.grid_world.shape, dtype=int)
exploration_bonus[0][0] = 0
# Produce an episode by taking actions until collision
while simulation_world.active:
    # Pass state to arduino to get action
    # Pass result of action to arduino for update
    print(simulation_world)
    if random.random() < epsilon:
        # action = int(input("Input action:"))
        action = random.choice(range(simulation_world.action_space))
    else:
        print("Calling arduino for action.")
        # call arduino for action
        # Get state, convert to int
        state_tuple = simulation_world.robot_loc
        state = state_tuple[0] * simulation_world.grid_width + state_tuple[1]
        action = get_action(state)
    # epsilon = max(min_epsilon, epsilon * (1 - epsilon_decay))
    # Decay epsilon
    s, a, r, s_prime = simulation_world.step(action)
    # self = simulation_world
    # Add 100 to r to avoid negatives (subtract 100 on the receiving end)
    r += 100
    # Add exploration bonus
    r += (exploration_bonus[s_prime[0]][s_prime[1]] * 100)
    # Remove bonus
    exploration_bonus[s_prime[0]][s_prime[1]] = 0
    # s_prime = (5, 5)
    # shaping_adjustment = int((sum(s_prime)**3) / 10)
    # if last_shaping_adjustment > shaping_adjustment:
    #     last_shaping_adjustment = shaping_adjustment
    #     shaping_adjustment = -10
    # else:
    #     last_shaping_adjustment = shaping_adjustment
    #     shaping_adjustment = 0
    # r += shaping_adjustment
    # Reward shaping: add state since the goal is at the largest state to encourage movement
    # towards there
    s = s[0] * simulation_world.grid_width + s[1]
    s_prime = s_prime[0] * simulation_world.grid_width + s_prime[1]
    # Call arduino to add experience
    add_experience(s, action, r, s_prime)
    sleep(.1)
print("Episode result:", simulation_world.total_points)
# Connect to arduino for action


# Testing the robot brain
simulation_world = World(seed=10)
random.seed(time())
# Produce an episode by taking actions until collision
while simulation_world.active:
    # TODO Pass state to arduino to get action
    # TODO Pass result of action to arduino for update
    print(simulation_world)
    print("Calling arduino for action.")
    # todo call arduino for action
    # Get state, convert to int
    state_tuple = simulation_world.robot_loc
    state = state_tuple[0] * simulation_world.grid_width + state_tuple[1]
    action = get_action(state)
    # Decay epsilon
    # epsilon = epsilon * (1 - epsilon_decay)
    s, a, r, s_prime = simulation_world.step(action)
    # self = simulation_world
    # Add 100 to r to avoid negatives (subtract 100 on the receiving end)
    r += 100
    s = s[0] * simulation_world.grid_width + s[1]
    s_prime = s_prime[0] * simulation_world.grid_width + s_prime[1]
    # TODO Call arduino to add experience
    add_experience(s, action, r, s_prime)
    sleep(.5)
print(simulation_world)
# Testing arduino Q-learner for basic functionality

# Given a world with 1 state and 2 actions
# there is a good action and a bad action
# can it at least learn that?

check = np.zeros(simulation_world.grid_world.shape, dtype=int)
for row in range(check.shape[0]):
    for col in range(check.shape[1]):
        s = row * simulation_world.grid_width + col
        a = get_action(s)
        print("Row", row, "Col", col, "State", s, "Action", a)
        check[row][col] = a
print(check)

reset(2)
test()
s = 0
# Show result of bad action
a = 0
r = -100 + 100
s_prime = 0
add_experience(s, a, r, s_prime)
test()
# Show result of good action
a = 1
r = 100 + 100
s_prime = 0
add_experience(s, a, r, s_prime)
test()
# Ask it what action it prefers...
get_action(s)
