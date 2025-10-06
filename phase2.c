#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 1
#define NUM_THREADS 3
#define TRANSACTIONS_PER_TELLER 5
#define INITIAL_BALANCE 1000.0

// Shared data structure with mutex
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock; // Mutex for this account
} Account;

// Global accounts array
Account accounts[NUM_ACCOUNTS];

// Deposit function (mutex-protected)
void deposit(int account_id, double amount) {
    pthread_mutex_lock(&accounts[account_id].lock);
    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;
    pthread_mutex_unlock(&accounts[account_id].lock);
}

// Withdraw function (mutex-protected)
void withdraw(int account_id, double amount) {
    pthread_mutex_lock(&accounts[account_id].lock);
    accounts[account_id].balance -= amount;
    accounts[account_id].transaction_count++;
    pthread_mutex_unlock(&accounts[account_id].lock);
}

// Thread function
void *teller_thread(void *arg) {
    int teller_id = *(int *)arg;

    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        double amount = (rand() % 200); // 0-199
        int deposit_flag = rand() % 2;

        if (deposit_flag) {
            printf("Thread %d: Depositing %.2f\n", teller_id, amount);
            deposit(0, amount);
        } else {
            printf("Thread %d: Withdrawing %.2f\n", teller_id, amount);
            withdraw(0, amount);
        }

        usleep(rand() % 1000);
    }

    return NULL;
}


int main() {
    srand(time(NULL));

    // Initialize account and mutex
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    printf("Initial balance: %.2f\n", accounts[0].balance);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final balance: %.2f\n", accounts[0].balance);
    printf("Total transactions: %d\n", accounts[0].transaction_count);

    // Destroy mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
