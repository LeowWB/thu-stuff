MIN_SUPPORT = 10000
IN_PATH = './processed.pickle'
OUT_PATH = './pattern_counts.pickle'
NUM_ACCS = 800000

import pickle
from datetime import datetime
from functools import reduce 

def flatten(list_of_lists):
    return reduce(lambda l1, l2: list(l1)+list(l2), list_of_lists, [])

def get_vertices_in_component(edges):
    return set(flatten(map(
        lambda edge: edge[:2],
        edges)))

def get_next_edge_choices(edges, all_relevant_edges, all_edges):
    return set(filter(
        lambda e: e > max(edges), # avoid duplicates
        map( # edges connected to component
            lambda edge_ind: all_edges[edge_ind],
            set(flatten(map( # set of indices of edges connected to component
                lambda vert: all_relevant_edges[vert],
                get_vertices_in_component(edges)))))))

def edge_to_metadata(edge):
    return edge[2:]

def get_loops(edges):
    return list(filter(lambda edge: edge[0] == edge[1], edges))

def sorted_metadata(edges):
    return list(sorted(map(edge_to_metadata, edges)))

# (pattern num, (edge1), (edge2), (edge3), (nodes))
def edge_triplet_to_pattern_tuple(edges, all_vertices):
    vertices_in_component = get_vertices_in_component(edges)
    loops = get_loops(edges)
    if len(vertices_in_component) == 1:
        pn = 0
        n = (vertices_in_component[0])
        es = sorted_metadata(edges)
        e1, e2, e3 = sorted(es)
    elif len(vertices_in_component) == 2:
        if len(loops) == 2:
            nonloop_edge = list(edges - set(loops))[0]
            if loops[0][0] == loops[1][0]:
                looping_v = loops[0][0]
                other_v = list(vertices_in_component - set([looping_v]))[0]
                if nonloop_edge[0] == other_v:
                    pn = 3
                    n = (other_v, looping_v)
                    e1 = edge_to_metadata(nonloop_edge)
                    e2, e3 = sorted_metadata(loops)
                else:
                    pn = 1
                    n = (looping_v, other_v)
                    e3 = edge_to_metadata(nonloop_edge)
                    e1, e2 = sorted_metadata(loops)
            else:
                pn = 2
                n = (nonloop_edge[0], nonloop_edge[1])
                if loops[0][0] == n[0]:
                    e1 = edge_to_metadata(loops[0])
                    e3 = edge_to_metadata(loops[1])
                else:
                    e1 = edge_to_metadata(loops[1])
                    e3 = edge_to_metadata(loops[0])
                e2 = edge_to_metadata(nonloop_edge)
        elif len(loops) == 1:
            loop = loops[0]
            looping_v = loop[0]
            other_v = vertices_in_component - set(looping_v)
            nle1, nle2 = edges - set(loops)
            if nle1[0] == nle2[0]:
                if nle1[0] == looping_v:
                    pn = 4
                    n = (looping_v, other_v)
                    e1 = edge_to_metadata(loop)
                    e2, e3 = sorted_metadata([nle1, nle2])
                else:
                    pn = 5
                    n = (other_v, looping_v)
                    e3 = edge_to_metadata(loop)
                    e1, e2 = sorted_metadata([nle1, nle2])
            else:
                pn = 6
                n = (looping_v, other_v)
                e1 = edge_to_metadata(loop)
                if nle1[0] == looping_v:
                    e2 = edge_to_metadata(nle1)
                    e3 = edge_to_metadata(nle2)
                else:
                    e2 = edge_to_metadata(nle2)
                    e3 = edge_to_metadata(nle1)
        elif len(loops) == 0:
            edges = list(edges)
            if edges[0][0] == edges[1][0] and edges[1][0] == edges[2][0]:
                pn = 7
                n = (edges[0][0], edges[0][1])
                e1, e2, e3 = sorted_metadata(edges)
            else:
                pn = 8
                sorted_e = list(sorted(edges))
                n = tuple(sorted_e[1][:2])
                if sorted_e[0][0] == n[0]:
                    e1, e2 = sorted_metadata(sorted_e[:2])
                    e3 = edge_to_metadata(sorted_e[2])
                else:
                    e1, e2 = sorted_metadata(sorted_e[1:])
                    e3 = edge_to_metadata(sorted_e[0])
    elif len(vertices_in_component) == 3:
        if len(loops) == 1:
            looping_e = loops[0]
            looping_v = looping_e[0]
            other_edges = list(edges - set(looping_e))
            if other_edges[0][0] == other_edges[1][0]:
                if other_edges[0][0] == looping_v:
                    pn = 15
                    e2 = edge_to_metadata(looping_e)
                    ed1, ed3 = other_edges
                    if ed1[2:] > ed3[2:]:
                        ed3, ed1 = other_edges
                    e1 = edge_to_metadata(ed1)
                    e3 = edge_to_metadata(ed3)
                    n = (ed1[1], looping_v, ed3[1])
                else:
                    pn = 14
                    e1 = looping_e
                    if other_edges[0][1] == looping_v:
                        e2 = other_edges[0]
                        e3 = other_edges[1]
                    else:
                        e2 = other_edges[1]
                        e3 = other_edges[0]
                    n = (looping_v, e3[0], e3[1])
                    e1, e2, e3 = map(edge_to_metadata, [e1, e2, e3])
            elif other_edges[0][1] == other_edges[1][1]:
                if other_edges[0][1] == looping_v:
                    pn = 13
                    e2 = edge_to_metadata(looping_e)
                    ed1, ed3 = other_edges
                    if ed1[2:] > ed3[2:]:
                        ed3, ed1 = other_edges
                    e1 = edge_to_metadata(ed1)
                    e3 = edge_to_metadata(ed3)
                    n = (ed1[0], looping_v, ed3[0])
                else:
                    pn = 12
                    e1 = looping_e
                    if other_edges[0][0] == looping_v:
                        e2 = other_edges[0]
                        e3 = other_edges[1]
                    else:
                        e2 = other_edges[1]
                        e3 = other_edges[0]
                    n = (looping_v, e3[1], e3[0])
                    e1, e2, e3 = map(edge_to_metadata, [e1, e2, e3])
        else:
            edges = list(edges)
            vs1, vs2, vs3 = map(lambda e: set(e[:2]), edges)
            if vs1 == vs2:
                parallels = edges[:2]
                nonpara = edges[2]
                para_vs = list(vs1)
            elif vs1 == vs3:
                parallels = [edges[0], edges[2]]
                nonpara = edges[1]
                para_vs = list(vs1)
            elif vs2 == vs3:
                parallels = edges[1:]
                nonpara = edges[0]
                para_vs = list(vs2)
            else:
                parallels = None
            if parallels:
                other_v = list(vertices_in_component - set(para_vs))[0]
                if parallels[0][0] == parallels[1][0]:
                    e1, e2 = sorted_metadata(parallels)
                    e3 = edge_to_metadata(nonpara)
                    if nonpara[0] == other_v:
                        if parallels[0][0] == nonpara[1]:
                            pn = 21
                            n = (parallels[0][1], parallels[0][0], other_v)
                        else:
                            pn = 17
                            n = (parallels[0][0], parallels[0][1], other_v)
                    else:
                        if parallels[0][0] == nonpara[0]:
                            pn = 20
                            n = (parallels[0][1], parallels[0][0], other_v)
                        else:
                            pn = 16
                            n = (parallels[0][0], parallels[0][1], other_v)
                else:
                    if nonpara[0] == other_v:
                        pn = 19
                        v2 = nonpara[1]
                    else:
                        pn = 18
                        v2 = nonpara[0]
                    if parallels[0][0] == v2:
                        e2 = parallels[0]
                        e1 = parallels[1]
                        v1 = parallels[0][1]
                    else:
                        e1 = parallels[0]
                        e2 = parallels[1]
                        v1 = parallels[0][0]
                    e3 = edge_to_metadata(nonpara)
                    e1, e2 = map(edge_to_metadata, [e1, e2])
                    n = (v1, v2, other_v)
            else:
                edges = list(edges)
                sources = list(map(lambda ed: ed[0], edges))
                if len(set(sources)) == 3:
                    pn = 22
                    edges = list(sorted(edges, key=lambda ed: ed[2:]))
                    e1, e2, e3 = map(edge_to_metadata, edges)
                    n = tuple(map(lambda ed: ed[0], edges))
                else:
                    pn = 21
                    sources.sort()
                    v1 = sources[1]
                    if sources[0] != v1:
                        v2 = sources[0]
                    else:
                        v2 = sources[2]
                    for ed in edges:
                        if ed[0] == v2:
                            ed2 = ed
                    edges.remove(ed2)
                    v3 = ed2[1]
                    n = (v1, v2, v3)
                    if edges[0][1] == v2:
                        ed1 = edges[0]
                        ed3 = edges[1]
                    else:
                        ed1 = edges[1]
                        ed3 = edges[0]
                    e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
    elif len(vertices_in_component) == 4:
        edges = list(edges)
        sources = list(map(lambda ed: ed[0], edges))
        dests = list(map(lambda ed: ed[1], edges))
        if len(set(sources)) == 3:
            if len(set(dests)) == 3:
                pn = 24
                v1 = list(set(sources) - set(dests))[0]
                v4 = list(set(dests) - set(sources))[0]
                for edge in edges:
                    if edge[0] == v1:
                        ed1 = edge
                    if edge[1] == v4:
                        ed3 = edge
                n = (v1, ed1[1], ed3[0], v4)
                edges.remove(ed1)
                edges.remove(ed3)
                ed2 = edges[0]
                e1, e2, e3 = map(edge_to_metadata, [ed1,ed2,ed3])
            elif len(set(dests)) == 2:
                dests = sorted(list(dests))
                indeg_2_v = dests[1]
                pn = 25
                for ed in edges:
                    if ed[0] == indeg_2_v:
                        indeg_2_outgoing_e = ed
                        pn = 30
                        break
                if pn == 25:
                    v3 = list(set(dests) - set(sources))[0]
                    for ed in edges:
                        if ed[1] != v3:
                            ed1 = ed
                    v2 = ed1[1]
                    for ed in edges:
                        if ed[0] == v2:
                            ed2 = ed
                    edges.remove(ed1)
                    edges.remove(ed2)
                    ed3 = edges[0]
                    e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
                    n = (ed1[0], ed2[0], ed2[1], ed3[0])
                elif pn == 30:
                    v3 = indeg_2_v
                    ed2 = indeg_2_outgoing_e
                    v2 = ed2[1]
                    edges.remove(ed2)
                    ed1, ed3 = sorted(edges, key=lambda ed: ed[2:])
                    e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
                    n = (ed1[0], v2, v3, ed3[0])
            else:
                pn = 31
                edges = list(sorted(edges, key=lambda ed: ed[2:]))
                e1, e2, e3 = map(edge_to_metadata, edges)
                n = (e1[0], e2[0], e2[1], e3[0])
        elif len(set(sources)) == 2:
            if len(set(dests)) == 3:
                sources = sorted(list(sources))
                outdeg_2_v = sources[1]
                pn = 27
                for ed in edges:
                    if ed[1] == outdeg_2_v:
                        outdeg_2_incoming_e = ed
                        pn = 29
                        break
                if pn == 27:
                    v2 = list(set(sources) - set(dests))[0]
                    for ed in edges:
                        if ed[0] != v2:
                            ed3 = ed
                    v3 = ed3[0]
                    for ed in edges:
                        if ed[1] == v3:
                            ed2 = ed
                    edges.remove(ed3)
                    edges.remove(ed2)
                    ed1 = edges[0]
                    e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
                    n = (ed1[1], ed2[0], ed2[1], ed3[1])
                elif pn == 29:
                    v3 = outdeg_2_v
                    ed3 = outdeg_2_incoming_e
                    v4 = ed3[0]
                    edges.remove(ed3)
                    ed1, ed2 = sorted(edges, key=lambda ed: ed[2:])
                    e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
                    n = (ed1[1], ed2[1], ed3[1], ed3[0])
            elif len(set(dests)) == 2:
                pn = 26
                sources = list(sorted(sources))
                dests = list(sorted(dests))
                v3 = sources[1]
                v2 = dests[1]
                for ed in edges:
                    if ed[1] == v2 and ed[0] != v3:
                        ed1 = ed
                    elif ed[1] == v2 and ed[0] == v3:
                        ed2 = ed
                    else:
                        ed3 = ed
                n = (ed1[0], ed1[1], ed3[0], ed3[1])
                e1, e2, e3 = map(edge_to_metadata, [ed1, ed2, ed3])
        else:
            pn = 28
            edges = list(sorted(edges, key=lambda ed: ed[2:]))
            e1, e2, e3 = map(edge_to_metadata, edges)
            n = (e1[1], e2[1], e2[0], e3[1])
    
    n = tuple(map(lambda ind: all_vertices[ind % NUM_ACCS], n)) # % NUM ACCS to reset the ids.
    return (pn, e1, e2, e3, n)

def register_edge_triplet(edges, pattern_counts, all_vertices):
    pattern_tuple = edge_triplet_to_pattern_tuple(edges, all_vertices)
    pattern_counts[pattern_tuple] = pattern_counts.get(pattern_tuple, 0) + 1
    if pattern_counts[pattern_tuple] == MIN_SUPPORT:
        print(f'{pattern_tuple} has just exceeded {MIN_SUPPORT}!=================================')

def write(pattern_counts):
    out_dict = {k:v for k,v in pattern_counts.items() if v >= MIN_SUPPORT}
    print(f'current count past threshold is {len(out_dict.keys())}')
    with open(OUT_PATH, 'wb') as f:
        pickle.dump(out_dict, f)


def main():
    print('reading')
    with open(IN_PATH, 'rb') as f:
        all_vertices, all_edges, all_relevant_edges = pickle.load(f) # edge: source, dest, amt, strat, bus
    print(f'finished reading at {datetime.now()}')

    pattern_counts = dict()
    for iter_num, edge1 in enumerate(all_edges):
        if iter_num % 10000 == 10:
            write(pattern_counts)
            print(f'outer loop on iter {iter_num} at {datetime.now()}')
        
        for edge2 in get_next_edge_choices([edge1], all_relevant_edges, all_edges):
            for edge3 in get_next_edge_choices([edge1, edge2], all_relevant_edges, all_edges):
                register_edge_triplet(set([edge1, edge2, edge3]), pattern_counts, all_vertices)
    
    print('writing')
    write(pattern_counts)
main()