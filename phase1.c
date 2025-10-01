#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>   // For usleep
#include <time.h>     // For random seed

#define NUM_ACCOUNTS 1
#define NUM_THREADS 3
#define TRANSACTIONS_PER_TELLER 5

// Shared data structure
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];

// Thread function
void *teller_thread(void *arg) {
    int teller_id = *(int *)arg;

    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        // Select account 0 (single account for phase 1)
        Account *acc = &accounts[0];

        // Random deposit or withdrawal
        double amount = (rand() % 200); // 0 - 199
        int deposit = rand() % 2;

        if (deposit) {
            printf("Thread %d: Depositing %.2f\n", teller_id, amount);
            acc->balance += amount;  // RACE CONDITION HERE
        } else {
            printf("Thread %d: Withdrawing %.2f\n", teller_id, amount);
            acc->balance -= amount;  // RACE CONDITION HERE
        }

        acc->transaction_count++;

        // Sleep a tiny random amount to increase chance of race
        usleep(rand() % 1000);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    // Initialize account
    accounts[0].account_id = 0;
    accounts[0].balance = 1000.0;
    accounts[0].transaction_count = 0;

    printf("Initial balance: %.2f\n", accounts[0].balance);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1; // Thread IDs start at 1
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final balance: %.2f\n", accounts[0].balance);
    printf("Total transactions: %d\n", accounts[0].transaction_count);

    return 0;
}

