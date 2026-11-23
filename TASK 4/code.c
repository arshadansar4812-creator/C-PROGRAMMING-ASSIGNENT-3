#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DISTRICTS 38
#define MONTHS 12

void compute_district_totals(int rows, int cols, const double rain[rows][cols], double totals[]) {
    for (int i = 0; i < rows; i++) {
        double sum = 0.0;
        for (int j = 0; j < cols; j++) {
            sum += rain[i][j];
        }
        totals[i] = sum;
    }
}

void compute_month_averages(int rows, int cols, const double rain[rows][cols], double month_avg[]) {
    for (int j = 0; j < cols; j++) {
        double sum = 0.0;
        for (int i = 0; i < rows; i++) {
            sum += rain[i][j];
        }
        month_avg[j] = (rows > 0) ? (sum / rows) : 0.0;
    }
}

void find_wettest_district_month(int rows, int cols, const double rain[rows][cols],
                                int *best_d, int *best_m, double *max_val) {
    *best_d = 0;
    *best_m = 0;
    *max_val = rain[0][0];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (rain[i][j] > *max_val) {
                *max_val = rain[i][j];
                *best_d = i;
                *best_m = j;
            }
        }
    }
}

double compute_state_annual_average(const double totals[], int districts) {
    double sum = 0.0;
    for (int i = 0; i < districts; i++) {
        sum += totals[i];
    }
    return (districts > 0) ? (sum / districts) : 0.0;
}

void print_deficient_districts(const double totals[], int districts, double threshold) {
    printf("Deficient districts (< %.2f mm annual rainfall):\n", threshold);
    int found = 0;
    for (int i = 0; i < districts; i++) {
        if (totals[i] < threshold) {
            printf("  District %d: %.2f mm\n", i + 1, totals[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("  None\n");
    }
}

static void verify_3x4(void) {
    double sample[3][4] = {
        {100.0, 120.0, 80.0, 100.0}, /* total = 400 */
        {200.0, 180.0, 220.0, 200.0}, /* total = 800 */
        { 50.0,  60.0, 40.0,  50.0}  /* total = 200 -> state avg annual = 1400/3 = 466.67, 60% = 280.0 */
    };
    double totals[3], m_avg[4], max_val;
    int bd, bm;

    compute_district_totals(3, 4, sample, totals);
    compute_month_averages(3, 4, sample, m_avg);
    find_wettest_district_month(3, 4, sample, &bd, &bm, &max_val);
    double state_avg = compute_state_annual_average(totals, 3);

    printf("=== 3x4 Verification ===\n");
    for (int i = 0; i < 3; i++) {
        printf("District %d total: %.2f\n", i + 1, totals[i]);
    }
    for (int j = 0; j < 4; j++) {
        printf("Month %d avg: %.2f\n", j + 1, m_avg[j]);
    }
    printf("Wettest: District %d, Month %d = %.2f\n", bd + 1, bm + 1, max_val);
    print_deficient_districts(totals, 3, 0.60 * state_avg);
    printf("\n");
}

static void benchmark_2000x2000(void) {
    int n = 2000;
    double (*grid)[2000] = malloc((size_t)n * sizeof(*grid));
    if (!grid) {
        perror("malloc 2000x2000 failed");
        return;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            grid[i][j] = (double)((i + j) % 100);
        }
    }

    volatile double sink = 0.0;
    clock_t t0 = clock();
    double row_sum = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            row_sum += grid[i][j];
        }
    }
    clock_t t1 = clock();

    double col_sum = 0.0;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            col_sum += grid[i][j];
        }
    }
    clock_t t2 = clock();
    sink = row_sum + col_sum;

    printf("=== 2000x2000 Traversal Speed (sink=%.0f) ===\n", sink);
    printf("Row-wise    : %.3f ms\n", (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC);
    printf("Column-wise : %.3f ms\n", (double)(t2 - t1) * 1000.0 / CLOCKS_PER_SEC);
    free(grid);
}

int main(void) {
    verify_3x4();

    double rain[DISTRICTS][MONTHS];
    srand(123);
    for (int i = 0; i < DISTRICTS; i++) {
        for (int j = 0; j < MONTHS; j++) {
            rain[i][j] = (double)(rand() % 250) + (double)(rand() % 100) / 100.0;
        }
    }

    double totals[DISTRICTS], month_avg[MONTHS], max_val;
    int best_d, best_m;
    compute_district_totals(DISTRICTS, MONTHS, rain, totals);
    compute_month_averages(DISTRICTS, MONTHS, rain, month_avg);
    find_wettest_district_month(DISTRICTS, MONTHS, rain, &best_d, &best_m, &max_val);
    double state_avg = compute_state_annual_average(totals, DISTRICTS);

    printf("=== 38x12 District Rainfall Summary ===\n");
    printf("State average annual rainfall: %.2f mm\n", state_avg);
    printf("Wettest slot: District %d, Month %d (%.2f mm)\n", best_d + 1, best_m + 1, max_val);
    print_deficient_districts(totals, DISTRICTS, 0.60 * state_avg);
    printf("\n");

    benchmark_2000x2000();
    return 0;
}
