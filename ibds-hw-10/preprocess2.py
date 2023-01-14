IN_PATH = './data'
OUT_PATH = './processed.pickle'

NAME_TO_INT = {
    'Jobs': 0,
    'John': 1,
    'Mike': 2
}

NUM_ACCS = 800000
NUM_CARDS = 600000

import os
import pickle

vertices = []
relevant_edges = []

vertices = [None] * (NUM_ACCS + NUM_CARDS)
for i in range(NUM_ACCS + NUM_CARDS):
    relevant_edges.append([])

name_freqs = [0,0,0]
amt_freqs = dict()
strat_freqs = dict()
buscode_freqs = dict()

print('reading account file')
with open(os.path.join(IN_PATH, 'account'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        vid, name = line.split(',')[:2]
        name = NAME_TO_INT[name]
        vertices[int(vid)] = name
        name_freqs[name] += 1

print('reading card file')
with open(os.path.join(IN_PATH, 'card'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        vid, name = line.split(',')[:2]
        name = NAME_TO_INT[name]
        vertices[int(vid) + NUM_ACCS] = name
        name_freqs[name] += 1

print('reading account_to_account file')
with open(os.path.join(IN_PATH, 'account_to_account'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        source, dest, _, amt, strat, __, buscode = line.split(',')[:7]
        source = int(source)
        dest = int(dest)
        amt = round(float(amt))
        strat = int(strat[-1])
        buscode = int(buscode[-1])
        new_edge = (source, dest, amt, strat, buscode)
        relevant_edges[source].append(new_edge)
        amt_freqs[amt] = amt_freqs.get(amt, 0) + 1
        strat_freqs[strat] = strat_freqs.get(strat, 0) + 1
        buscode_freqs[buscode] = buscode_freqs.get(buscode, 0) + 1
        

print('reading account_to_card file')
with open(os.path.join(IN_PATH, 'account_to_card'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        source, dest, _, amt, strat, __, buscode = line.split(',')[:7]
        source = int(source)
        dest = int(dest) + NUM_ACCS
        amt = round(float(amt))
        strat = int(strat[-1])
        buscode = int(buscode[-1])
        new_edge = (source, dest, amt, strat, buscode)
        relevant_edges[source].append(new_edge)
        amt_freqs[amt] = amt_freqs.get(amt, 0) + 1
        strat_freqs[strat] = strat_freqs.get(strat, 0) + 1
        buscode_freqs[buscode] = buscode_freqs.get(buscode, 0) + 1

print('writing')
with open(OUT_PATH, 'wb') as f:
    pickle.dump((vertices, relevant_edges), f, protocol=pickle.HIGHEST_PROTOCOL)

print('done.')
print('stats:')
print(name_freqs)
for fr in [amt_freqs, strat_freqs, buscode_freqs]:
    for k in sorted(fr.keys()):
        print(f'{k}\t{fr[k]}')
    print('\n')
