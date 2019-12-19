# Copyright 2019 DeepMind Technologies Ltd. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Python spiel example."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import tensorflow as tf
from open_spiel.python.algorithms import dqn
#from open_spiel.python.algorithms import dqn_np as dqn

import logging
from absl import app
from absl import flags

from open_spiel.python import rl_environment
from open_spiel.python.algorithms import random_agent

FLAGS = flags.FLAGS

flags.DEFINE_string("game", "landlord", "Name of the game.")
flags.DEFINE_integer("num_players", 3, "Number of players.")
flags.DEFINE_integer("num_episodes", 100000,"Number of episodes.")
#flags.DEFINE_integer("num_episodes", 100,"Number of episodes.")

flags.DEFINE_integer(
    "eval_every", 1000,
    "Episode frequency at which the DQN agents are evaluated.")
flags.DEFINE_string("checkpoint_dir", "/tmp/landlord/",
                    "Directory to save/load the agent.")

def print_iteration(time_step, player_id, action=None):
  """Print TimeStep information."""
  obs = time_step.observations
  logging.info("Player: %s", player_id)
  if time_step.first():
    logging.info("Info state: %s, - - %s", obs["info_state"][player_id],
                 time_step.step_type)
  else:
    logging.info("Info state: %s, %s %s %s", obs["info_state"][player_id],
                 time_step.rewards[player_id], time_step.discounts[player_id],
                 time_step.step_type)
  if action is not None:
    logging.info("Action taken: %s", action)
  logging.info("-" * 80)


def main_loop(unused_arg):
    """RL main loop example."""
    logging.info("Registered games: %s", rl_environment.registered_games())
    logging.info("Creating game %s", FLAGS.game)

    #env_configs = {"players": FLAGS.num_players} if FLAGS.num_players else {}
    env_configs =  {}
    env = rl_environment.Environment(FLAGS.game, **env_configs)
    state_size = env.observation_spec()["info_state"][0]
    num_actions = env.action_spec()["num_actions"]

    hidden_layers_sizes = [512, 512]
    replay_buffer_capacity = int(1e4)
    train_episodes = FLAGS.num_episodes
    loss_report_interval = 1000

    logging.info("Env specs: %s", env.observation_spec())
    logging.info("Action specs: %s", env.action_spec())

    with tf.Session() as sess:
        agents = [
            dqn.DQN(  # pylint: disable=g-complex-comprehension
                sess,
                player_id,
                state_representation_size=state_size,
                num_actions=num_actions,
                #hidden_layers_sizes=[16],
                #replay_buffer_capacity=10,
                hidden_layers_sizes=hidden_layers_sizes,
                replay_buffer_capacity=replay_buffer_capacity,
                batch_size=128) for player_id in range(3)
        ]
        saver = tf.train.Saver()
        sess.run(tf.global_variables_initializer())
        #latest_checkpoint_path = tf.train.latest_checkpoint(FLAGS.checkpoint_dir)
        latest_checkpoint_path = tf.train.latest_checkpoint(FLAGS.checkpoint_dir)
        if latest_checkpoint_path:
            print('Restoring checkpoint: {0}'.format(latest_checkpoint_path))
            saver.restore(sess,latest_checkpoint_path)
        
         # Train agent
        for ep in range(train_episodes):
            #if ep and ep % loss_report_interval == 0:
            if (ep + 1) % FLAGS.eval_every == 0:
                logging.info("[%s/%s] DQN loss: %s   %s  %s", ep, train_episodes, agents[0].loss, agents[1].loss, agents[2].loss)
                saver.save(sess, FLAGS.checkpoint_dir, ep)
                
            time_step = env.reset()
            while not time_step.last():
                current_player = time_step.observations["current_player"]
                #agent_output = [agent.step(time_step) for agent in agents]
                #time_step = env.step([agent_output[current_player].action])
                if env.is_turn_based:
                    agent_output = agents[current_player].step(time_step)
                    action_list = [agent_output.action]
                else:
                    agents_output = [agent.step(time_step) for agent in agents]
                    action_list = [agent_output.action for agent_output in agents_output]
                #print_iteration(time_step, current_player, action_list)
                time_step = env.step(action_list)

            # Episode is over, step all agents with final info state.
            for agent in agents:
                agent.step(time_step)

            # Print final state of end game.
            # for pid in range(env.num_players):
            #     print_iteration(time_step, pid)


if __name__ == "__main__":
  app.run(main_loop)
