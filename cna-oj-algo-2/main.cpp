#include <cstdio>
#include <cstdlib>
using namespace std;

// return true if can find subset of elements in k that sum to goal. N is number of elems left to check.
bool canFindSubset(int* k, int N, int goal) {
    if (N == 0) {
        return goal == 0;
    }

    if (goal < 0) {
        return false;
    }

    if (goal == 0) {
        return true;
    }

    return (canFindSubset(k, N-1, goal-k[N-1]) || canFindSubset(k, N-1, goal));
}

int main() {
    int N;
    scanf("%d", &N);
    int k[N];
    int sum=0;
    for (int i = 0; i < N; i++) {
        scanf("%d", &k[i]);
        sum += k[i];
    }
    if (sum%2 == 1) {
        printf("N");
        return 0;
    }
    if (canFindSubset(k, N, sum/2)) {
        printf("Y");
    } else {
        printf("N");
    }
    return 0;
}