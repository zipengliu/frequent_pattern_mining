#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#define FILENAME_LEN 100
#define MAX_THRESHOLD 10
#define MAX_TRANSACTIONS 100001
#define MAX_ITEMS_OF_ONE_TRANS 80
#define MAX_ITEMS 17000
#define MAX_LINE 500

typedef struct {
    int *items;
    int sup;
} item_set_t;

double thresholds[MAX_TRANSACTIONS];
int num_thresholds;
int data[MAX_TRANSACTIONS][MAX_ITEMS_OF_ONE_TRANS];
item_set_t *candidates, *frequent_sets;
item_set_t *all_frequent_sets;
int num_candidates, num_frequent_sets, max_k;
int num_transactions, num_items[MAX_TRANSACTIONS], max_items;

void input_data(const char *data_file);
void input_config(const char *config_file);
void output_frequent_set(char *output_file);
void sort_data();
void apriori(int threshold);
void generate_candidate(int k);
void filter_candidates(int k, double threshold);
void copy_item_set(item_set_t *dst, item_set_t *src, int k);
void sup_count(int k);
int is_contained(int *items, int k, int *data, int n);
int can_combine(int *a, int *b, int n);


int main(int argc, const char *argv[])
{
    clock_t start_time, end_time;
    const char USAGE[] = "./apriori [DATA_FILE] [CONFIG_FILE] [OUTPUT_FILE]";
    int i;

    if (argc != 4) {
        printf("%s\n", USAGE);
        exit(1);
    }
    input_data(argv[1]);
    input_config(argv[2]);
    sort_data();        // TODO: should this be included into time count?

    for (i = 0; i < num_thresholds; i++) {
        start_time = clock();

        apriori(thresholds[i]);

        end_time = clock();
        printf("threshold=%lf time elapsed=%ld\n", thresholds[i], 1000 * (end_time - start_time) / CLOCKS_PER_SEC);

        char output_file[FILENAME_LEN];
        sprintf(output_file, "%s_%d", argv[3], i);
        output_frequent_set(output_file);
    }

    return 0;
}

void input_data(const char *data_file) {
    FILE *f = fopen(data_file, "r");
    if (f == NULL) {
        printf("%s not found\n", data_file);
        exit(1);
    }

    char buf[MAX_LINE], *tmp;
    max_items = 0;
    num_transactions = 0;
    memset(num_items, 0, sizeof(num_items));
    while (fgets(buf, MAX_LINE, f)) {
        tmp = strtok(buf, " ");
        while (tmp != NULL) {
            int t = atoi(tmp);
            if (t > max_items) max_items = t;
            data[num_transactions][num_items[num_transactions]] = t;
            num_items[num_transactions]++;
            tmp = strtok(NULL, " ");
        }
        num_transactions++;
    }
#ifdef DEBUG
    printf("number of transactions: %d\n", num_transactions);
    printf("number of items: %d\n", max_items);
#endif
}

void input_config(const char *config_file) {
    FILE *f = fopen(config_file, "r");
    if (f == NULL) {
        printf("%s not found\n", config_file);
        exit(1);
    }

    num_thresholds = 0;
    while (fscanf(f, "%lf", &thresholds[num_thresholds]) != EOF) {
        num_thresholds++;
    }
}

void output_frequent_set(char *output_file) {
    int i, j;
    for (i = 0; i < num_frequent_sets; i++) {
        for (j = 0; j < max_k; j++)
            printf("%d ", frequent_sets[i].items[j]);
        printf("%d\n", frequent_sets[i].sup);
    }
}

void sort_data() {
    // insert sort for small number of items
    int i, j, k, t;
    for (k = 0; k < num_transactions; k++) {
        for (i = 0; i < num_items[k]; i++) {
            for (j = i + 1; j < num_items[k]; j++)
                if (data[k][i] > data[k][j]) {
                    t = data[k][i];
                    data[k][i] = data[k][j];
                    data[k][j] = t;
                }
        }
    }
}

void sup_count(int k) {
    int i, j;
    int *items;
    for (i = 0; i < num_candidates; i++) {
        items = candidates[i].items;
        candidates[i].sup = 0;
        for (j = 0; j < num_transactions; j++)
            candidates[i].sup += is_contained(items, k, data[j], num_items[j]);
    }
}

int is_contained(int *items, int k, int *data, int n) {
    if (k > n) return 0;
    int i = 0, j = 0, s = 0;
    while (i < k && j < n) {
        if (items[i] < items[j])
            i++;
        else if (items[i] > items[j])
            j++;
        else {
            i++;
            j++;
            s++;
        }
    }
    return (s == k);
}

int can_combine(int *a, int *b, int n) {
    int i;
    for (i = 0; i < n - 1; i++) {
        if (a[i] != b[i]) return 0;
    }
    return (a[n - 1] < b[n - 1]);
}

void generate_candidates(int k) {
    int i, j;
    if (k == 1) {        // 1-item set
        candidates = malloc((max_items + 1) * sizeof(item_set_t));
        for (i = 0; i <= max_items; i++) {
            candidates[i].items = malloc(sizeof(int));
            candidates[i].items[0] = i;
        }
        num_candidates = max_items + 1;
        return;
    }

    int max_num_candidates = num_frequent_sets * (num_frequent_sets - 1) / 2;
    candidates = realloc(candidates, max_num_candidates * sizeof(item_set_t));
    num_candidates = 0;
    for (i = 0; i < num_frequent_sets - 1; i++)
        for (j = i + 1; j < num_frequent_sets; j++)
            if (can_combine(frequent_sets[i].items, frequent_sets[j].items, k - 1)) {
                candidates[num_candidates].items = realloc(candidates[num_candidates].items, k * sizeof(int));
                memcpy(candidates[num_candidates].items, frequent_sets[i].items, (k - 1) * sizeof(int));
                candidates[num_candidates].items[k - 1] = frequent_sets[j].items[k - 2];
                num_candidates++;
            }
}

void copy_item_set(item_set_t *dst, item_set_t *src, int k) {
    dst->items = realloc(dst->items, k * sizeof(int));
    memcpy(dst->items, src->items, k * sizeof(int));
    dst->sup = src->sup;
}

void filter_candidates(int k, double threshold) {
    int min_sup = threshold * num_transactions / 100;
    int i;
    frequent_sets = realloc(frequent_sets, num_candidates * sizeof(item_set_t));
    num_frequent_sets = 0;
    for (i = 0; i < num_candidates; i++)
        if (candidates[i].sup >= min_sup) {
            copy_item_set(&frequent_sets[num_frequent_sets], &candidates[i], k);
            num_frequent_sets++;
        }
}

void apriori(int threshold) {
    int k = 1;
    generate_candidates(k);

    while (k <= max_items && num_candidates > 0) {
        sup_count(k);
        filter_candidates(k, threshold);
#ifdef DEBUG
        printf("k = %d\t candidates: %d\t frequent sets:%d\n", k, num_candidates, num_frequent_sets);
#endif
        k++;
        generate_candidates(k);
    }
    max_k = k - 1;
}
