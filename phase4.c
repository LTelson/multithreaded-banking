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
// Deadlock-free transfer function using consistent lock ordering
void *transfer_safe(void *arg) {
    TransferArgs *args = (TransferArgs *)arg;
    int from = args->from_id;
    int to = args->to_id;
    double amount = args->amount;

    int first = from < to ? from : to;
    int second = from < to ? to : from;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    accounts[from].balance -= amount;
    accounts[to].balance += amount;

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);

    printf("Thread %ld: Completed safe transfer from %d to %d amount %.2f\n",
           pthread_self(), from, to, amount);

    return NULL;
}
int main() {
    srand(time(NULL));

    // Initialize accounts and mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    // Phase 1/2 style threads performing random transactions
    pthread_t tellers[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&tellers[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Wait for teller threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tellers[i], NULL);
    }

    // Phase 4: Safe transfers
    TransferArgs t1_args = {0, 1, 150};
    TransferArgs t2_args = {1, 0, 200};

    pthread_t t1, t2;
    pthread_create(&t1, NULL, transfer_safe, &t1_args);
    pthread_create(&t2, NULL, transfer_safe, &t2_args);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Print final balances
    printf("\nFinal balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: %.2f (Transactions: %d)\n",
               accounts[i].account_id,
               accounts[i].balance,
               accounts[i].transaction_count);
    }

    // Destroy mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
