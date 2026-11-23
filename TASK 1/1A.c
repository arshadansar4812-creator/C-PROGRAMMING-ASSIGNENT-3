#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int linear_search(const int a[], int n, int key, long long *comparisons) {
    *comparisons = 0;
    for (int i = 0; i < n; i++) {
        (*comparisons)++;
        if (a[i] == key) {
            return i;
        }
    }
    return -1;
}

int binary_search_count(const int a[], int n, int key, long long *comparisons) {
    int low = 0, high = n - 1;
    *comparisons = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        (*comparisons)++;
        if (a[mid] == key) {
            return mid;
        } else if (a[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

static void run_trial_10(void) {
    int unsorted[] = {1305, 1021, 1460, 1190, 1533, 1048, 1377, 1103, 1402, 1222};
    int sorted[]   = {1021, 1048, 1103, 1190, 1222, 1305, 1377, 1402, 1460, 1533};
    int n = 10;
    int test_keys[] = {1021, 1305, 1533, 9999}; /* first, middle, last, missing */
    const char *labels[] = {"Beginning", "Middle", "End", "Missing"};

    printf("=== Trial Catalogue (n = 10) ===\n");
    for (int i = 0; i < 4; i++) {
        long long lin_cmp = 0, bin_cmp = 0;
        int lin_idx = linear_search(unsorted, n, test_keys[i], &lin_cmp);
        int bin_idx = binary_search_count(sorted, n, test_keys[i], &bin_cmp);
        printf("%-10s (key=%4d): Linear -> idx=%2d, cmp=%2lld | Binary -> idx=%2d, cmp=%2lld\n",
               labels[i], test_keys[i], lin_idx, lin_cmp, bin_idx, bin_cmp);
    }
    printf("\n");
}

static void run_scale_experiment(int n) {
    int *sorted = malloc((size_t)n * sizeof(int));
    int *unsorted = malloc((size_t)n * sizeof(int));
    if (!sorted || !unsorted) {
        perror("malloc failed");
        free(sorted);
        free(unsorted);
        return;
    }

    for (int i = 0; i < n; i++) {
        sorted[i] = 1000 + i * 2;
        unsorted[i] = sorted[i];
    }
    /* Shuffle unsorted array using Fisher-Yates */
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = unsorted[i];
        unsorted[i] = unsorted[j];
        unsorted[j] = tmp;
    }

    long long lin_hit_cmp = 0, lin_miss_cmp = 0;
    long long bin_hit_cmp = 0, bin_miss_cmp = 0;
    int trials = 200;

    for (int t = 0; t < trials; t++) {
        long long c = 0;
        int target = sorted[rand() % n];
        linear_search(unsorted, n, target, &c);
        lin_hit_cmp += c;

        linear_search(unsorted, n, -1, &c);
        lin_miss_cmp += c;

        binary_search_count(sorted, n, target, &c);
        bin_hit_cmp += c;

        binary_search_count(sorted, n, -1, &c);
        bin_miss_cmp += c;
    }

    printf("n = %6d | Avg Linear Hit: %7.2f, Miss: %7lld | Avg Binary Hit: %5.2f, Miss: %5lld\n",
           n,
           (double)lin_hit_cmp / trials,
           lin_miss_cmp / trials,
           (double)bin_hit_cmp / trials,
           bin_miss_cmp / trials);

    free(sorted);
    free(unsorted);
}

int main(void) {
    srand((unsigned int)time(NULL));
    run_trial_10();
    printf("=== Scaling Experiment ===\n");
    run_scale_experiment(1000);
    run_scale_experiment(10000);
    run_scale_experiment(100000);
    return 0;
}
