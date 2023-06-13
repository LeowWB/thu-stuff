#include <cstdio>
#include <cstdlib>
using namespace std;

const int BASE = 1000000000 + 7;

int min(int a, int b){
    if (a<b)
        return a;
    else
        return b;
}

int main() {
    int x1, x2, y1, y2;
    scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

    if ((x1-y1) * (x2-y2) <= 0) { // has to cross the line no matter what.
        printf("%d", 0);
        return 0;
    }

    if ((x1-y2) * (x2-y1) > 0) { // the shortest path will never cross the line no matter what.

        int x_dist = abs(x1-x2);
        int y_dist = abs(y1-y2);

        // so it's just ncr

        int n = x_dist + y_dist;
        int r = x_dist;

        // 0-indexed

        int cur_row[n+1];
        int prev_row[n+1];

        cur_row[0] = 1;

        for (int i = 1; i <= n; i++) {
            for (int j = 0; j < n+1; j++) {

                prev_row[j] = cur_row[j];
            }
            cur_row[0] = 1;
            for (int j = 1; j < i; j++) {
                cur_row[j] = (prev_row[j] + prev_row[j-1]) % BASE;
            }
            cur_row[i] = 1;
        }

        int ncr = cur_row[r];
        printf("%d", ncr);
        return 0;
    }
    else { // there is the risk of the shortest path crossing the line.
        
        int x_dist = abs(x1-x2);
        int y_dist = abs(y1-y2);

        int n = x_dist + y_dist;
        int r = x_dist;
        int other_r = min(abs(y2-x1), abs(x2-y1));

        int cur_row[n+1];
        int prev_row[n+1];

        cur_row[0] = 1;

        for (int i = 1; i <= n; i++) {
            for (int j = 0; j < n+1; j++) {

                prev_row[j] = cur_row[j];
            }
            cur_row[0] = 1;
            for (int j = 1; j < i; j++) {
                cur_row[j] = (prev_row[j] + prev_row[j-1]) % BASE;
            }
            cur_row[i] = 1;
        }

        int rv = cur_row[r] - cur_row[other_r];
        if (rv < 0) {
            rv += BASE;
        }
        printf("%d", rv);
        return 0;
    }
}