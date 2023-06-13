MIN_SUPPORT = 10000
IN_PATH = './processed.pickle'
OUT_PATH = './pattern_counts.pickle'
NUM_ACCS = 800000

import pickle
from datetime import datetime
from functools import reduce 

def register_edge_triplet(edges, pattern_counts, all_vertices):
    verts = [edges[0][0], edges[1][0], edges[2][0], edges[2][1]]
    if len(set(verts)) < 4:
        return
    pattern_tuple = (
        tuple(map(lambda v: all_vertices[v], verts)),
        tuple(map(lambda ed: ed[2:], edges))
    )
    pattern_counts[pattern_tuple] = pattern_counts.get(pattern_tuple, 0) + 1
    if pattern_counts[pattern_tuple] == MIN_SUPPORT:
        print(f'{pattern_tuple} has just exceeded {MIN_SUPPORT}!=================================')

def write(pattern_counts):
    out_dict = {k:v for k,v in pattern_counts.items() if v >= MIN_SUPPORT}
    print(f'current count past threshold is {len(out_dict.keys())}')
    with open(OUT_PATH, 'wb') as f:
        pickle.dump(out_dict, f)

ok_amts = set([0,1,2])
ok_strats = set([2,3,4])
ok_buscodes = set([1,2,3])
def shouldnt_skip(edge):
    return (
        (edge[2] in ok_amts) and
        (edge[3] in ok_strats) and
        (edge[4] in ok_buscodes)
    )

def main():
    print('reading')
    with open(IN_PATH, 'rb') as f:
        all_vertices, all_outgoing_edges = pickle.load(f) # edge: source, dest, amt, strat, bus
    print(f'finished reading at {datetime.now()}')

    pattern_counts = dict()
    for start_vid in range(len(all_vertices)):
        if start_vid % 10000 == 10:
            print(f'outer loop on iter {start_vid} at {datetime.now()}')
            write(pattern_counts)
            print(f'autosaved')
            print(f'current max count is {max(pattern_counts.values())}')

        for edge1 in filter(shouldnt_skip, all_outgoing_edges[start_vid]):
            for edge2 in filter(shouldnt_skip, all_outgoing_edges[edge1[1]]):
                for edge3 in filter(shouldnt_skip, all_outgoing_edges[edge2[1]]):
                    register_edge_triplet([edge1, edge2, edge3], pattern_counts, all_vertices)
    
    print('writing')
    write(pattern_counts)
main()