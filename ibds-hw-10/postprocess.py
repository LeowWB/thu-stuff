'''
    if anything goesa wrong, maybe u wanna try including itmestamp. i ncase the saem edge exists twice but w diff timestamp. doing that will increase num of edges.
    but this isn't necessary i think. cuz not many missing (9420703 vs 9278130). can say in report - could have done this but wld drastically icnrease amt of data cuz
    timestamps big numbers too and python dynamicaly assigns size i think? and not for much benefit i think.

'''

IN_PATH = './pattern_counts.pickle'
OUT_PATH = './bdci_data.json'
MIN_SUPPORT = 10000

import pickle
import json

def get_edge_map(pn):
    return [
        [(1,1), (1,1), (1,1)], #0
        [(1,1), (1,1), (1,2)], #1
        [(1,1), (1,2), (2,2)], #2
        [(1,2), (2,2), (2,2)], #3
        [(1,1), (1,2), (1,2)], #4
        [(1,2), (1,2), (2,2)], #5
        [(1,1), (1,2), (2,1)], #6
        [(1,2), (1,2), (1,2)],
        [(1,2), (1,2), (2,1)],
        [(1,1), (1,2), (2,3)],
        [(1,2), (2,2), (2,3)], # 10
        [(1,2), (2,3), (3,3)],
        [(1,1), (1,2), (3,2)],
        [(1,2), (2,2), (3,2)],
        [(1,1), (2,1), (2,3)],
        [(2,1), (2,2), (2,3)], # 15
        [(1,2), (1,2), (2,3)],
        [(1,2), (1,2), (3,2)],
        [(1,2), (2,1), (2,3)],
        [(1,2), (2,1), (3,2)],
        [(2,1), (2,1), (2,3)], # 20
        [(2,1), (2,1), (3,2)],
        [(1,2), (2,3), (3,1)],
        [(1,2), (2,3), (1,3)],
        [(1,2), (2,3), (3,4)],
        [(1,2), (2,3), (4,3)], #25
        [(1,2), (3,2), (3,4)],
        [(2,1), (2,3), (3,4)],
        [(3,1), (3,2), (3,4)],
        [(3,1), (3,2), (4,3)],
        [(1,3), (3,2), (4,3)], # 30
        [(1,3), (2,3), (4,3)]
    ][pn]

def get_next_entry(pat):
    pn, e1, e2, e3, n = pat
    e = [e1, e2, e3]
    es = []
    ns = []
    for i, name_ind in enumerate(n):
        ns.append({
            'node_id': i+1,
            'name': ['Jobs', 'John', 'Mike'][name_ind]
        })
    edge_map = get_edge_map(pn)
    for i in range(3):
        es.append({
            'source_node_id': edge_map[i][0],
            'target_node_id': edge_map[i][1],
            'amt': float(e[i][2])
            'strategy_name': str(e[i][3]),
            'buscode': str(e[i][4])
        })
    rv = {'nodes': ns, 'edges': es}

output_list = []

with open(IN_PATH, 'rb') as f:
    pattern_counts = pickle.load(f)

for k in pattern_counts.keys():
    if pattern_counts[k] < MIN_SUPPORT:
        continue
    output_list.append(get_next_entry(k))

with open(OUT_PATH, 'w') as f:
    json.dump(output_list, f)
