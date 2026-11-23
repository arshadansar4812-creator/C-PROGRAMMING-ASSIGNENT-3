#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    long long comparisons;
    long long swaps_or_shifts;
    double elapsed_ms;
} SortStats;

static void swap_int(int *x, int *y, SortStats *st) {
    int t = *x;
    *x = *y;
    *y = t;
    st->swaps_or_shifts++;
}

SortStats bubble_sort(int a[], int n) {
    SortStats st = {0, 0, 0.0};
    clock_t start = clock();
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            st.comparisons++;
            if (a[j] > a[j + 1]) {
                swap_int(&a[j], &a[j + 1], &st);
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
    st.elapsed_ms = (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
    return st;
}

SortStats selection_sort(int a[], int n) {
    SortStats st = {0, 0, 0.0};
    clock_t start = clock();
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            st.comparisons++;
            if (a[j] < a[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap_int(&a[i], &a[min_idx], &st);
        }
    }
    st.elapsed_ms = (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
    return st;
}

SortStats insertion_sort(int a[], int n) {
    SortStats st = {0, 0, 0.0};
    clock_t start = clock();
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            st.comparisons++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                st.swaps_or_shifts++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;
    }
    st.elapsed_ms = (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
    return st;
}

static void generate_input(int a[], int n, int mode) {
    /* mode: 0 = sorted, 1 = nearly sorted, 2 = reverse, 3 = random */
    for (int i = 0; i < n; i++) {
        a[i] = 10000 + i;
    }
    if (mode == 1) {
        int perturbations = (n > 20) ? 20 : (n / 5);
        for (int p = 0; p < perturbations; p++) {
            int idx1 = rand() % n;
            int idx2 = rand() % n;
            int tmp = a[idx1];
            a[idx1] = a[idx2];
            a[idx2] = tmp;
        }
    } else if (mode == 2) {
        for (int i = 0; i < n; i++) {
            a[i] = 10000 + (n - 1 - i);
        }
    } else if (mode == 3) {
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }
}

static void print_array(const int a[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d%s", a[i], (i + 1 < n) ? " " : "\n");
    }
}

int main(void) {
    srand(42);

    /* Hand-trace sample of 6 register numbers */
    int trace_base[6] = {105, 102, 109, 101, 106, 103};
    int buf[6];
    printf("=== 6-Element Sample Trace ===\nInitial: ");
    print_array(trace_base, 6);

    memcpy(buf, trace_base, sizeof(trace_base));
    SortStats s_bub = bubble_sort(buf, 6);
    printf("Bubble    -> cmp=%lld, swaps=%lld | ", s_bub.comparisons, s_bub.swaps_or_shifts);
    print_array(buf, 6);

    memcpy(buf, trace_base, sizeof(trace_base));
    SortStats s_sel = selection_sort(buf, 6);
    printf("Selection -> cmp=%lld, swaps=%lld | ", s_sel.comparisons, s_sel.swaps_or_shifts);
    print_array(buf, 6);

    memcpy(buf, trace_base, sizeof(trace_base));
    SortStats s_ins = insertion_sort(buf, 6);
    printf("Insertion -> cmp=%lld, shifts=%lld | ", s_ins.comparisons, s_ins.swaps_or_shifts);
    print_array(buf, 6);

    /* Benchmark across sizes and input orders */
    int sizes[] = {100, 1000, 10000};
    const char *modes[] = {"Sorted", "Nearly-Sorted", "Reverse", "Random"};

    printf("\n=== Benchmark Results ===\n");
    for (int s = 0; s < 3; s++) {
        int n = sizes[s];
        int *base = malloc((size_t)n * sizeof(int));
        int *work = malloc((size_t)n * sizeof(int));
        for (int m = 0; m < 4; m++) {
            generate_input(base, n, m);

            memcpy(work, base, (size_t)n * sizeof(int));
            SortStats b = bubble_sort(work, n);

            memcpy(work, base, (size_t)n * sizeof(int));
            SortStats sel = selection_sort(work, n);

            memcpy(work, base, (size_t)n * sizeof(int));
            SortStats ins = insertion_sort(work, n);

            printf("n=%5d | %-13s | Bub: (%8lld, %8lld, %6.2fms) | Sel: (%8lld, %6lld, %6.2fms) | Ins: (%8lld, %8lld, %6.2fms)\n",
                   n, modes[m],
                   b.comparisons, b.swaps_or_shifts, b.elapsed_ms,
                   sel.comparisons, sel.swaps_or_shifts, sel.elapsed_ms,
                   ins.comparisons, ins.swaps_or_shifts, ins.elapsed_ms);
        }
        free(base);
        free(work);
    }
    return 0;
}
