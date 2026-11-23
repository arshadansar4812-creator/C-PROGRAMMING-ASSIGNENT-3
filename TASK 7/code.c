#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CANDIDATES 12
#define MAX_VOTES 500000

typedef struct {
    int candidate_id;
    int votes;
} CandidateResult;

void tally_votes(const int votes[], int num_votes, int num_candidates,
                 int counts[], int *valid_count, int *invalid_count) {
    for (int c = 1; c <= num_candidates; c++) {
        counts[c] = 0;
    }
    *valid_count = 0;
    *invalid_count = 0;

    for (int i = 0; i < num_votes; i++) {
        int v = votes[i];
        if (v >= 1 && v <= num_candidates) {
            counts[v]++;
            (*valid_count)++;
        } else {
            (*invalid_count)++;
        }
    }
}

void sort_results_desc(const int counts[], int num_candidates, CandidateResult results[]) {
    for (int c = 1; c <= num_candidates; c++) {
        results[c - 1].candidate_id = c;
        results[c - 1].votes = counts[c];
    }

    /* Descending by votes; tie-breaker ascending by candidate_id */
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - 1 - i; j++) {
            int swap_needed = 0;
            if (results[j].votes < results[j + 1].votes) {
                swap_needed = 1;
            } else if (results[j].votes == results[j + 1].votes &&
                       results[j].candidate_id > results[j + 1].candidate_id) {
                swap_needed = 1;
            }
            if (swap_needed) {
                CandidateResult tmp = results[j];
                results[j] = results[j + 1];
                results[j + 1] = tmp;
            }
        }
    }
}

void report_election(const char *label, const int votes[], int num_votes, int num_candidates) {
    int counts[MAX_CANDIDATES + 1];
    CandidateResult results[MAX_CANDIDATES];
    int valid = 0, invalid = 0;

    clock_t start = clock();
    tally_votes(votes, num_votes, num_candidates, counts, &valid, &invalid);
    sort_results_desc(counts, num_candidates, results);
    double elapsed_ms = (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    printf("=== %s (total=%d, candidates=%d, time=%.3f ms) ===\n",
           label, num_votes, num_candidates, elapsed_ms);
    printf("Valid votes: %d | Invalid votes: %d\n", valid, invalid);

    if (num_votes <= 30) {
        for (int i = 0; i < num_candidates; i++) {
            printf("  Candidate %2d : %d votes\n", results[i].candidate_id, results[i].votes);
        }
    }

    if (valid == 0) {
        printf("Outcome: No valid votes cast.\n\n");
        return;
    }

    int top_votes = results[0].votes;
    int tie_count = 0;
    for (int i = 0; i < num_candidates; i++) {
        if (results[i].votes == top_votes) {
            tie_count++;
        } else {
            break;
        }
    }

    if (tie_count > 1) {
        printf("Outcome: TIE among %d candidates with %d votes each:", tie_count, top_votes);
        for (int i = 0; i < tie_count; i++) {
            printf(" Candidate %d", results[i].candidate_id);
        }
        printf("\n\n");
    } else {
        printf("Outcome: Winner is Candidate %d with %d votes.\n\n",
               results[0].candidate_id, top_votes);
    }
}

int main(void) {
    /* 1. Hand-verifiable small set with invalid votes and a tie */
    int small_votes[] = {1, 3, 2, 0, -4, 3, 2, 99, 1, 5};
    report_election("Small Tie + Invalid Test", small_votes, 10, 5);

    /* 2. 5,000 votes benchmark */
    int *votes_5k = malloc(5000 * sizeof(int));
    if (votes_5k) {
        srand(7);
        for (int i = 0; i < 5000; i++) {
            votes_5k[i] = (rand() % 16) - 2; /* includes -2..0 and 13..14 as invalid */
        }
        report_election("5,000 Votes Run", votes_5k, 5000, 12);
        free(votes_5k);
    }

    /* 3. 500,000 votes benchmark */
    int *votes_500k = malloc((size_t)MAX_VOTES * sizeof(int));
    if (votes_500k) {
        srand(19);
        for (int i = 0; i < MAX_VOTES; i++) {
            votes_500k[i] = (rand() % 16) - 2;
        }
        report_election("500,000 Votes Run", votes_500k, MAX_VOTES, 12);
        free(votes_500k);
    }

    return 0;
}
