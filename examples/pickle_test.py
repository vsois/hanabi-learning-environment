from hanabi_learning_environment import pyhanabi_pybind as pyhanabi
from hanabi_multiagent_framework.utils import make_hanabi_env_config
import pickle

game_params = make_hanabi_env_config()
game = pyhanabi.HanabiGame(game_params)

card = pyhanabi.HanabiCard(pyhanabi.HanabiCard.ColorType.kBlue,
                           pyhanabi.HanabiCard.RankType.k1)
knowledge = pyhanabi.CardKnowledge(5, 5)

hand = pyhanabi.HanabiHand()
hand.add_card(card, knowledge)

move1 = pyhanabi.HanabiMove(pyhanabi.HanabiMove.Type.kRevealColor, -1, 1, 1, -1)
move2 = pyhanabi.HanabiMove(pyhanabi.HanabiMove.Type.kDiscard, 0, 0, 0, 0)

hist = pyhanabi.HanabiHistoryItem(move2)
hist.player= 0
hist.color = 1
hist.rank = 2

state = pyhanabi.HanabiState(game)
while state.current_player==-1:
    state.apply_random_chance()
state.apply_move(move1)
#state.apply_move(move2)

    
obs = pyhanabi.HanabiObservation(state, 1)
print(state)
print(obs)

# pickle test
pickle.dump(obs, open( "save.p", "wb" ) )
obj = pickle.load( open( "save.p", "rb" ) )
print(obj)



