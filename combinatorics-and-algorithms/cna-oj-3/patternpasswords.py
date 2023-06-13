import sys
from copy import copy
a,b,l = list(map(int, sys.stdin.readline().strip().split(' ')))

nums_with_stuff_between = {(1,3),(1,7),(1,9),(2,8),(3,7),(3,9),(4,6),(7,9)}
def has_num_between(x, y):
    m, n = min(x,y), max(x,y)
    return (m,n) in nums_with_stuff_between
def get_num_between(x, y):
    return (x+y)/2


def get_options(path_so_far):
    options = []
    for i in range(1, 10):
        if i in path_so_far:
            continue
        if has_num_between(path_so_far[-1], i) and (get_num_between(path_so_far[-1], i) not in path_so_far):
            continue
        options.append(i)
    return options

def go(b, l, path_so_far, rv):
    if len(path_so_far) == l:
        if path_so_far[-1] == b:
            #print(path_so_far)
            rv[0] += 1
        return
    for opt in get_options(path_so_far):
        go(b, l, copy(path_so_far)+[opt], rv)

def doit(a,b,l):
    rv=[0]
    go(b, l, [a], rv)
    return rv[0]

print(doit(a,b,l))
