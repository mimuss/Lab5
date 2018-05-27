#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <getopt.h>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
long long result = 1;

struct func_val
{
    long long start;
    long long end;
    long long mod;
    long long pnum;
};

void* thread_factorial(void* a)
{
    struct func_val *t = (struct func_val*)a;
    long long start = t->start;
    long long end = t->end;
    long long mod = t->mod;
    long long pnum =  t->pnum;
    //printf("%lld %lld\n",start, end);
    for(long long i = start; i<=end; i+=pnum)
    {
        pthread_mutex_lock(&mutex1);
        result = (result * (i%mod))%mod;
        pthread_mutex_unlock(&mutex1);
        
    }
};


int main(int argc, char **argv)
{
    long long k = -1;
    long long mod = -1;
    int pnum = -1;
    while (true)
    {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"k", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "k", options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            switch (option_index)
            {
            case 0:
                k = atoi(optarg);
                break;
            case 1:
                mod = atoi(optarg);
                if (mod <= 0)
                {
                    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",argv[0]);
                    return 1;
                }
                break;
            case 2:
                pnum = atoi(optarg);
                if (pnum < 0)
                {
                    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",argv[0]);
                    return 1;
                }
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'k':
            k = atoi(optarg);
            break;

        case '?':
            break;

        default:
            printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc)
    {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (k == -1 || mod == -1 || pnum == -1)
    {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
               argv[0]);
        return 1;
    }
    pthread_t *threads = malloc(pnum*sizeof(pthread_t));
    struct func_val* t = malloc(pnum*sizeof(struct func_val));;
    for(int i = 1; i<=pnum; i++)
    {
        t[i].start = i;
        t[i].end = k;
        t[i].mod = mod;
        t[i].pnum = pnum;        
        pthread_create( &threads[i], NULL, thread_factorial, &t[i]);
    }
    for(int i = 0; i<pnum; i++)
    {
        pthread_join( threads[i], NULL);
    }
    printf("factorial %lld modulo %lld = %lld\n",k, mod, result);
    fflush(NULL);
    return 0;
}