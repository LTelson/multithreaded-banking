#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSACTIONS_PER_THREAD 5
#define INITIAL_BALANCE 1000

// Account structure with mutex
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;  // Mutex for this account
} Account;

Account accounts[NUM_ACCOUNTS];

// Structure for transfer arguments
typedef struct {
    int from_id;
    int to_id;
    double amount;
} TransferArgs;

// Thread function: deposits/withdrawals to demonstrate mutex protection
void *teller_thread(void *arg) {
    int tid = *(int *)arg;

    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        int account_id = rand() % NUM_ACCOUNTS;
        double amount = (rand() % 200) - 100;  // Random deposit/withdrawal -100..+100

        pthread_mutex_lock(&accounts[account_id].lock);
        accounts[account_id].balance += amount;
        accounts[account_id].transaction_count++;
        pthread_mutex_unlock(&accounts[account_id].lock);

        printf("Thread %d: Transaction %d on account %d amount %.2f\n",
               tid, i, account_id, amount);
        usleep(50);  // simulate processing delay
    }

    return NULL;
}
