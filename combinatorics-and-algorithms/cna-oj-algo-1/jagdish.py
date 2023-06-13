import sys
N, M = map(int, sys.stdin.readline().strip().split(' '))
all_skills = []
for i in range(N):
    all_skills.append( int(sys.stdin.readline().strip()))

answers = []
for bleh in range(M):
    range_lower, range_upper = map(int, sys.stdin.readline().strip().split(' '))

    skills = all_skills[range_lower:range_upper+1]
    
    partial_sum = 0
    partial_sums = [0]
    lowest_partial_sum_so_far = 0
    lowest_partial_sums_by_certain_points = [0]
    for skill in skills:
        partial_sum += skill
        partial_sums.append(partial_sum)
        lowest_partial_sum_so_far = min(lowest_partial_sum_so_far, partial_sum)
        lowest_partial_sums_by_certain_points.append(lowest_partial_sum_so_far)
    
    max_ans = -99999999
    for i in range(1, len(partial_sums)):
        next_res = partial_sums[i] - lowest_partial_sums_by_certain_points[i-1]
        max_ans = max(next_res, max_ans)

    answers.append(max_ans)

print('\n'.join(map(str, answers)), end='')