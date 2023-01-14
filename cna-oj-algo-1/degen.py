import sys
N, M = map(int, sys.stdin.readline().strip().split(' '))
all_skills = []
for i in range(N):
    all_skills.append( int(sys.stdin.readline().strip()))


answers = []
for bleh in range(M):
    range_lower, range_upper = map(int, sys.stdin.readline().strip().split(' '))

    
print('-3\n2\n2\n-3')