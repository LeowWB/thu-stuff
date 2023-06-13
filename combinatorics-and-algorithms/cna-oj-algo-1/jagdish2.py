import sys
N, M = map(int, sys.stdin.readline().strip().split(' '))
all_skills = []
for i in range(N):
    all_skills.append( int(sys.stdin.readline().strip()))

for bleh in range(M):
    range_lower, range_upper = map(int, sys.stdin.readline().strip().split(' '))

    partial_sum = 0
    lowest_partial_sum_so_far = 0
    lowest_ps_last_iter = 0
    max_ans = -99999999
    for skill in all_skills[range_lower:range_upper+1]:
        partial_sum += skill
        lowest_partial_sum_so_far = min(lowest_partial_sum_so_far, partial_sum)
        max_ans = max(max_ans, partial_sum - lowest_ps_last_iter)
        lowest_ps_last_iter = lowest_partial_sum_so_far
    print(max_ans)