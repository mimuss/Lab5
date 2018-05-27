#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef unsigned long long ull;

struct FactorialPart{
	ull start;
	ull finish;
	ull result;
};
void getFactorialPart(struct FactorialPart *factorialPart);

int result = 1;

pthread_t * threads;

ull mod = -1;

int main(int argc, char **argv) {
	ull k = -1;
	int pnum = -1;

	while (1) {
        int current_optind = optind ? optind : 1;
		static struct option options[] = {{"k", required_argument, 0, 0},
										  {"pnum", required_argument, 0, 0},
										  {"mod", required_argument, 0, 0},
										  {0, 0, 0, 0}};

		int option_index = 0;
		int c = getopt_long(argc, argv, "f", options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 0:
				switch (option_index) {
						case 0:
							k = atoi(optarg);
							if (k <= 0) {
								printf("k is a positive number\n");
								return 1;
							}
							break;
						case 1:
							pnum = atoi(optarg);
							if (pnum <= 0) {
								printf("pnum is a positive number\n");
								return 1;
							}
							break;
						case 2:
							mod = atoi(optarg);
							if (mod <= 0) {
								printf("mod is a positive number\n");
								return 1;
							}
							break;
						defalut:
							printf("Index %d is out of options\n", option_index);
				}
				break;
			default:
				printf("getopt returned character code 0%o?\n", c);
		}
	}

	if (optind < argc) {
		printf("Has at least one no option argument\n");
		return 1;
	}

	if (k == -1 || pnum == -1 || mod == -1) {
		printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n", argv[0]);
		return 1;
	}

	if (pnum >= k) {
		printf("pnum >= k\n");
		return 1;
	}

	struct FactorialPart * parts = malloc(pnum * sizeof(struct FactorialPart));
	threads = malloc(pnum * sizeof(pthread_t));


	parts[0].result = 1;
	parts[0].start = 1;
	int interval = k / pnum;
	for (int i = 1; i < pnum; i++){
		int start = i * k / pnum;
		parts[i - 1].finish = start;
		parts[i].start = start + 1;
	    parts[i].result = 1;
	}
	parts[pnum - 1].finish = k;

	
	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	for (int i = 0; i < pnum; i++){
		if (pthread_create(&threads[i], NULL, (void *)getFactorialPart,	(void *)&parts[i]) != 0) {
			exit(1);
		}
	}

	for (int i = 0; i < pnum; i++){
		if (pthread_join(threads[i], NULL) != 0) {
			exit(1);
		}
	}

	ull result = 1;

	for (int i = 0; i < pnum; i++)
		result = (result * parts[i].result) % mod;

	struct timeval finish_time;
	gettimeofday(&finish_time, NULL);

	double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
	elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

	printf("result = %llu\n", result);
	printf("Elapsed time: %fms\n", elapsed_time);
	

	return 0;
}

void getFactorialPart(struct FactorialPart *part) {
	for (ull i = part->start; i <= part->finish; i++){
		part->result = (part->result * i) % mod;
	}
}
