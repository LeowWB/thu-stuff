#naive

import sys
N, M = map(int, sys.stdin.readline().strip().split(' '))
all_skills = []
for i in range(N):
    all_skills.append( int(sys.stdin.readline().strip()))


answers = []
for bleh in range(M):
    range_lower, range_upper = map(int, sys.stdin.readline().strip().split(' '))

    skills = all_skills[range_lower:range_upper+1]
    max_ans = -9999999

    for i in range(len(skills)):
        for j in range(i+1, max(i+2,len(skills))):
            summi = sum(skills[i:j])
            max_ans = max(max_ans, summi)

    answers.append(max_ans)

for ans in answers:
    print(ans)