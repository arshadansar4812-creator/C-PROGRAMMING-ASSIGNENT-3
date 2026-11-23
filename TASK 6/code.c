#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAMES 5000
#define NAME_LEN 30

typedef struct {
    long long strcmp_calls;
    long long char_copies;
} DirStats;

int case_insensitive_cmp(const char *a, const char *b) {
    while (*a && *b) {
        unsigned char ca = (unsigned char)tolower((unsigned char)*a);
        unsigned char cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb) {
            return (int)ca - (int)cb;
        }
        a++;
        b++;
    }
    return (int)(unsigned char)tolower((unsigned char)*a) -
           (int)(unsigned char)tolower((unsigned char)*b);
}

void sort_by_copying(char arr[][NAME_LEN], int n, DirStats *st) {
    char temp[NAME_LEN];
    st->strcmp_calls = 0;
    st->char_copies = 0;

    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            st->strcmp_calls++;
            if (strcmp(arr[j], arr[j + 1]) > 0) {
                strcpy(temp, arr[j]);
                strcpy(arr[j], arr[j + 1]);
                strcpy(arr[j + 1], temp);
                st->char_copies += 3LL * (NAME_LEN / 2); /* approximate avg string copy work */
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

void sort_by_indices(char arr[][NAME_LEN], int idx[], int n, DirStats *st) {
    st->strcmp_calls = 0;
    st->char_copies = 0;
    for (int i = 0; i < n; i++) {
        idx[i] = i;
    }

    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            st->strcmp_calls++;
            if (strcmp(arr[idx[j]], arr[idx[j + 1]]) > 0) {
                int tmp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = tmp;
                st->char_copies++; /* counting index swaps instead of character copies */
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

int binary_search_indexed(char arr[][NAME_LEN], const int idx[], int n,
                          const char *target, long long *comparisons) {
    int low = 0, high = n - 1;
    *comparisons = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        (*comparisons)++;
        int cmp = strcmp(arr[idx[mid]], target);
        if (cmp == 0) {
            return idx[mid];
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

void report_duplicates_indexed(char arr[][NAME_LEN], const int idx[], int n) {
    printf("Duplicate entries (adjacent after sorting):\n");
    int found = 0;
    for (int i = 1; i < n; i++) {
        if (strcmp(arr[idx[i - 1]], arr[idx[i]]) == 0) {
            printf("  Duplicate: \"%s\"\n", arr[idx[i]]);
            found = 1;
        }
    }
    if (!found) {
        printf("  None\n");
    }
}

static void run_for_size(int n) {
    char (*names)[NAME_LEN] = malloc((size_t)n * sizeof(*names));
    char (*copy_arr)[NAME_LEN] = malloc((size_t)n * sizeof(*copy_arr));
    int *idx = malloc((size_t)n * sizeof(int));
    if (!names || !copy_arr || !idx) {
        perror("malloc failed");
        free(names);
        free(copy_arr);
        free(idx);
        return;
    }

    for (int i = 0; i < n; i++) {
        /* Create reverse-like and occasional duplicate names */
        snprintf(names[i], NAME_LEN, "Resident_%04d", n - i);
    }
    if (n >= 5) {
        snprintf(names[n - 1], NAME_LEN, "%s", names[0]);
    }

    memcpy(copy_arr, names, (size_t)n * sizeof(*names));

    DirStats st_copy, st_idx;
    sort_by_copying(copy_arr, n, &st_copy);
    sort_by_indices(names, idx, n, &st_idx);

    long long hit_cmp = 0, miss_cmp = 0;
    char probe[NAME_LEN];
    snprintf(probe, NAME_LEN, "Resident_%04d", n / 2);
    int found_idx = binary_search_indexed(names, idx, n, probe, &hit_cmp);
    int miss_idx = binary_search_indexed(names, idx, n, "Resident_9999", &miss_cmp);

    printf("n=%4d | CopySort cmp=%8lld, ~char_copies=%8lld | IdxSort cmp=%8lld, idx_swaps=%6lld | Search hit=%2d (%lld cmp), miss=%2d (%lld cmp)\n",
           n, st_copy.strcmp_calls, st_copy.char_copies,
           st_idx.strcmp_calls, st_idx.char_copies,
           found_idx, hit_cmp, miss_idx, miss_cmp);

    free(names);
    free(copy_arr);
    free(idx);
}

int main(void) {
    /* Length 29 vs length 30 buffer behavior */
    char s29[NAME_LEN];
    char s30[NAME_LEN];
    memset(s29, 'A', 29);
    s29[29] = '\0';
    snprintf(s30, NAME_LEN, "%s", "123456789012345678901234567890"); /* 30 chars -> truncated to 29 + '\0' */
    printf("Len 29 strlen=%zu | Len 30 truncated strlen=%zu\n\n", strlen(s29), strlen(s30));

    run_for_size(100);
    run_for_size(500);
    run_for_size(5000);

    /* Demonstration of fgets newline stripping + duplicates */
    char demo[6][NAME_LEN] = {"Arun", "Priya", "arun", "Priya", "Karthik", "Divya"};
    int d_idx[6];
    DirStats d_st;
    sort_by_indices(demo, d_idx, 6, &d_st);
    printf("\nSorted directory (case-sensitive strcmp):\n");
    for (int i = 0; i < 6; i++) {
        printf("  %s\n", demo[d_idx[i]]);
    }
    report_duplicates_indexed(demo, d_idx, 6);
    printf("Case-insensitive compare (\"apple\" vs \"Apple\"): %d\n",
           case_insensitive_cmp("apple", "Apple"));
    return 0;
}
