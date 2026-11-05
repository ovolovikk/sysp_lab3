#include <stdio.h>

int main() {
    int n = 10;
    int a = 0;
    int b = 1;
    int i;

    printf("fib:");
    for (i = 0; i < n; i++) {
        printf(" %d", a);
        int t = a + b;
        a = b;
        b = t;
    }
    printf("\n");

    int f = 1;
    int k = 1;
    while (k <= 5) {
        f = f * k;
        k = k + 1;
    }
    printf("5! = %d\n", f);

    int x = -7;
    printf("%d / 2 = %d, %d %% 2 = %d\n", x, x / 2, x, x % 2);
    printf("2 + 3 * 4 = %d\n", 2 + 3 * 4);

    if (f > 100 && !(n == 0)) {
        printf("big\n");
    } else if (f > 10) {
        printf("medium\n");
    } else {
        printf("small\n");
    }

    return 0;
}
