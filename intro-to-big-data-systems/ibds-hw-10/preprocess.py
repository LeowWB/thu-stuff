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

vertices = [] # indexed by vertex id. card ids have 800k added. each elem is a number representing the name
edges = [] # edge is (source, dest, amt, strat, bus)
relevant_edges = [] # for each vertex, the indices of the incoming and outgoing edges.

vertices = [None] * (NUM_ACCS + NUM_CARDS)
for i in range(NUM_ACCS + NUM_CARDS):
    relevant_edges.append([])

print('reading account file')
with open(os.path.join(IN_PATH, 'account'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        vid, name = line.split(',')[:2]
        vertices[int(vid)] = NAME_TO_INT[name]

print('reading card file')
with open(os.path.join(IN_PATH, 'card'), 'r') as f:
    lines = f.readlines()
    for line in lines:
        vid, name = line.split(',')[:2]
        vertices[int(vid) + NUM_ACCS] = NAME_TO_INT[name]

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
        edges.append(new_edge)
        relevant_edges[source].append(len(edges) - 1)
        relevant_edges[dest].append(len(edges) - 1)

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
        edges.append(new_edge)
        relevant_edges[source].append(len(edges) - 1)
        relevant_edges[dest].append(len(edges) - 1)

print(f'edges: {len(edges)}')
print('writing')
with open(OUT_PATH, 'wb') as f:
    pickle.dump((vertices, edges, relevant_edges), f, protocol=pickle.HIGHEST_PROTOCOL)

print('done.')