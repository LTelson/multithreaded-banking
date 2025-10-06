#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2

// Account structure with mutex
typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

// Structure to pass multiple arguments to transfer threads
typedef struct {
    int from_id;
    int to_id;
    double amount;
} TransferArgs;

// Transfer function that can deadlock
void *transfer(void *arg) {
    TransferArgs *args = (TransferArgs *)arg;
    int from = args->from_id;
    int to = args->to_id;
    double amount = args->amount;

    printf("Thread %ld: Attempting transfer from %d to %d\n",
           pthread_self(), from, to);

    // Lock the first account
    pthread_mutex_lock(&accounts[from].lock);
    printf("Thread %ld: Locked account %d\n", pthread_self(), from);

    // Simulate processing delay
    usleep(100);

    printf("Thread %ld: Waiting for account %d\n", pthread_self(), to);
    pthread_mutex_lock(&accounts[to].lock);

    // Critical section: transfer money
    accounts[from].balance -= amount;
    accounts[to].balance += amount;

    pthread_mutex_unlock(&accounts[to].lock);
    pthread_mutex_unlock(&accounts[from].lock);

    printf("Thread %ld: Completed transfer from %d to %d\n",
           pthread_self(), from, to);

    return NULL;
}
