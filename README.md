# Multithreaded Banking System

Phase 1 Approach:
I decided to opt for relying on three threads and a lower numbers of transactions per teller.
I wanted to be able to reasonably calculate the expected final balance by hand.
With the banking scenario, there would be a balance between "enough threads to show a race"
and "few enough threads to calculate by hand".

Phase 2 Approach:
An extension of phase 1, so since I got race conditions to be met with 3 threads and 5 transactions per teller (thread),
I stuck with that so the outputs between both phases could be comparable.
Mutexes (from skeleton) were introduced, so only one thread could modify
the account at a time. Mutexes were intialized before creating threads and are closed after all threads are finished.

Phase 3 Approach:
My goal was to get deadlock to occur reliably with smaller sample sizes,
such as 5:1 or 10:10. Inserting small usleep() delays throughout would increase
the chances that the other thread locks the first account in between. This
would display the mutexes violating lock ordering.

Phase 4 Approach:
My goal was safe transfers with no deadlock using lock ordering.
Mutex protection ensures that the balance and transaction count are updates atomically.
Race conditions are prevents, so there would be correct balances per output.
The smaller ID is locked first, then the second account is locked.
The transfer is performed and then unlocking is done in reverse order.
