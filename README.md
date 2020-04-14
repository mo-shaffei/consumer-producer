# consumer-producer
A semaphore based solution to synchronize one producer and many consumers with different rates of consumption and production.
This program is a UNIX specific program.

The producer and consumer are two completley separate programs synchronized through two semaphored, and sharing a memory segment protected by one semaphore(no forking involved).

The user gets to choose the production rate of the producer(items per second) and the buffer length. The buffer is a shared memory segment. Each item is an integer whose value is equal to the free buffer length at the time of production.
The user gets to choose the rate of consumption of the consumer(items per second) through the command line arguments.
