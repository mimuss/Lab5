#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void myFunc1();
void myFunc2();

pthread_t thread1, thread2;

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char **argv) {
	if (pthread_create(&thread1, NULL, (void *)myFunc1, NULL) != 0) 
		exit(1);
	if (pthread_create(&thread2, NULL, (void *)myFunc2, NULL) != 0)
		exit(1);

	if (pthread_join(thread1, NULL) != 0)
		exit(1);
	if (pthread_join(thread2, NULL) != 0)
		exit(1);

	return 0;
}

void myFunc1() {
    int lock1 = pthread_mutex_lock(&mut1);
    printf("Thread 1, lock 1 = %d\n", lock1);
    fflush(NULL);
    for (int i = 0; i < 9999999; i++);
    int lock2 = pthread_mutex_lock(&mut2);
    printf("Thread 1, lock 2 = %d\n", lock2);
    fflush(NULL);
    
    pthread_mutex_unlock(&mut1);
    pthread_mutex_unlock(&mut2);
}

void myFunc2() {
    int lock1 = pthread_mutex_lock(&mut2);
    printf("Thread 2, lock 1 = %d\n", lock1);
    fflush(NULL);
    
    for (int i = 0; i < 9999999; i++);
    int lock2 = pthread_mutex_lock(&mut1);
    printf("Thread 2, lock 2 = %d\n", lock2);
    fflush(NULL);
    
    pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut1);
}