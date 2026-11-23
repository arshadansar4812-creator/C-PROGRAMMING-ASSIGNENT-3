#include <stdio.h>

/* Merges two DESCENDING sorted arrays a[] and b[] into out[] in DESCENDING order. */
int merge_descending(const int a[], int na, const int b[], int nb, int out[], long long *comparisons) {
    int i = 0, j = 0, k = 0;
    if (comparisons) {
        *comparisons = 0;
    }

    while (i < na && j < nb) {
        if (comparisons) {
            (*comparisons)++;
        }
        /* >= preserves stability: equal scores from a[] come before b[] */
        if (a[i] >= b[j]) {
            out[k++] = a[i++];
        } else {
            out[k++] = b[j++];
        }
    }

    while (i < na) {
        out[k++] = a[i++];
    }
    while (j < nb) {
        out[k++] = b[j++];
    }
    return k;
}

static int is_sorted_descending(const int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] < arr[i]) {
            return 0;
        }
    }
    return 1;
}

static void print_result(const char *label, const int out[], int n, long long cmp) {
    printf("%-22s -> count=%2d, cmp=%2lld, valid_desc=%d | ",
           label, n, cmp, is_sorted_descending(out, n));
    for (int i = 0; i < n; i++) {
        printf("%d ", out[i]);
    }
    printf("\n");
}

int main(void) {
    long long cmp = 0;
    int out[20];

    int a1[] = {98, 91, 85, 72};
    int b1[] = {95, 91, 80};
    int n1 = merge_descending(a1, 4, b1, 3, out, &cmp);
    print_result("Different lengths/ties", out, n1, cmp);

    int a2[] = {90, 82, 75};
    int b2[] = {0};
    int n2 = merge_descending(a2, 3, b2, 0, out, &cmp);
    print_result("Second list empty", out, n2, cmp);

    int a3[] = {0};
    int b3[] = {88, 77};
    int n3 = merge_descending(a3, 0, b3, 2, out, &cmp);
    print_result("First list empty", out, n3, cmp);

    int a4[] = {85, 85, 85};
    int b4[] = {85, 85};
    int n4 = merge_descending(a4, 3, b4, 2, out, &cmp);
    print_result("All scores equal", out, n4, cmp);

    return 0;
}
