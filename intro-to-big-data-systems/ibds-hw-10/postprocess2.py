IN_PATH = './pattern_counts.pickle'
OUT_PATH = './bdci_data.json'

import pickle
import json

def get_next_entry(pattern):
    return {
        'nodes': [
            {
                'node_id': i,
                'name': ['Jobs', 'John', 'Mike'][pattern[0][i]]
            }
            for i in range(len(pattern[0]))],
        'edges': [
            {
                'source_node_id': i,
                'target_node_id': i+1,
                'amt': float(pattern[1][i][0]),
                'strategy_name': str(pattern[1][i][1]),
                'buscode': str(pattern[1][i][2])

            }
            for i in range(len(pattern[1]))]
    }
    
output_list = []

with open(IN_PATH, 'rb') as f:
    pattern_counts = pickle.load(f)

for k in pattern_counts.keys():
    output_list.append(get_next_entry(k))

with open(OUT_PATH, 'w') as f:
    json.dump(output_list, f, indent=4)
