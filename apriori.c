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
#define MAX_ITEMS 80
#define MAX_LINE 500

const char USAGE[] = "./apriori [DATA_FILE] [CONFIG_FILE] [OUTPUT_FILE]";
double thresholds[MAX_TRANSACTIONS];
int num_thresholds;
int data[MAX_TRANSACTIONS][MAX_ITEMS];
int num_transactions, num_items[MAX_TRANSACTIONS];

void input_data(const char *data_file);
void input_config(const char *config_file);
void output_frequent_set(char *output_file);
void apriori(int threshold);

int main(int argc, const char *argv[])
{
    clock_t start_time, end_time;
    int i;

    if (argc != 4) {
        printf("%s\n", USAGE);
        exit(1);
    }
    input_data(argv[1]);
    input_config(argv[2]);

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
    num_transactions = 0;
    memset(num_items, 0, sizeof(num_items));
    while (fgets(buf, MAX_LINE, f)) {
        tmp = strtok(buf, " ");
        while (tmp != NULL) {
            data[num_transactions][num_items[num_transactions]] = atoi(tmp);
            num_items[num_transactions]++;
            tmp = strtok(NULL, " ");
        }
        num_transactions++;
    }
#ifdef DEBUG
    printf("number of transactions: %d\n", num_transactions);
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

}

void apriori(int threshold) {

}
