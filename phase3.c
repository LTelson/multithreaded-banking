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

int main() {
    // Initialize accounts
    accounts[0].account_id = 0;
    accounts[0].balance = 1000;
    pthread_mutex_init(&accounts[0].lock, NULL);

    accounts[1].account_id = 1;
    accounts[1].balance = 1000;
    pthread_mutex_init(&accounts[1].lock, NULL);

    // Setup transfer arguments
    TransferArgs t1_args = {0, 1, 100};
    TransferArgs t2_args = {1, 0, 200};

    pthread_t t1, t2;

    // Create threads that will deadlock
    pthread_create(&t1, NULL, transfer, &t1_args);
    pthread_create(&t2, NULL, transfer, &t2_args);

    // Join threads (will hang due to deadlock)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final balances:\n");
    printf("Account 0: %.2f\n", accounts[0].balance);
    printf("Account 1: %.2f\n", accounts[1].balance);

    // Destroy mutexes
    pthread_mutex_destroy(&accounts[0].lock);
    pthread_mutex_destroy(&accounts[1].lock);

    return 0;
}
