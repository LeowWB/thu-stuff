#include <stdio.h>
#include <stdlib.h>
int min(int a, int b) {
    return a<b?a:b;
}
int max(int a, int b){
    return a>b?a:b;
}
int main() {
    int N;
    int M;
    int partial_sum;
    int lowest_partial_sum_so_far;
    int lowest_ps_last_iter;
    int max_ans;
    int range_lower, range_upper;
    scanf("%d %d", &N, &M);
    
    int all_skills[N];

    for (int i = 0; i < N; i++) {
        scanf("%d", &all_skills[i]);
    }
    
    for (int bleh = 0; bleh < M; bleh++) {
        scanf("%d %d", &range_lower, &range_upper);
        
        partial_sum = 0;
        lowest_partial_sum_so_far = 0;
        lowest_ps_last_iter = 0;
        max_ans = -99999999;

        for (int i = range_lower; i < range_upper+1; i++) {
            partial_sum += all_skills[i];
            lowest_partial_sum_so_far = min(lowest_partial_sum_so_far, partial_sum);
            max_ans = max(max_ans, partial_sum - lowest_ps_last_iter);
            lowest_ps_last_iter = lowest_partial_sum_so_far;

        }

        printf("%d\n", max_ans);
    }
    return 0;
}