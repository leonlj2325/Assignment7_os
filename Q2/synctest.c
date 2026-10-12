#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define NTHREADS 3
#define LOOPS 100000
#define BUFSIZE 5
#define ITEMS 20

static volatile int counter = 0;
static char counter_stacks[NTHREADS][PGSIZE] __attribute__((aligned(PGSIZE)));
static char pc_stacks[2][PGSIZE] __attribute__((aligned(PGSIZE)));
static int mutex_sem, empty_sem, full_sem;
static int buffer[BUFSIZE], in = 0, out = 0;

static void
counter_worker(void *arg)
{
  (void)arg;
  for (int i = 0; i < LOOPS; i++) {
    sem_wait(mutex_sem);
    counter++;
    sem_post(mutex_sem);
  }
  exit(0);
}

static void
producer(void *arg)
{
  (void)arg;
  for (int i = 1; i <= ITEMS; i++) {
    sem_wait(empty_sem);
    sem_wait(mutex_sem);
    buffer[in] = i;
    in = (in + 1) % BUFSIZE;
    printf("Producer inserted %d\n", i);
    sem_post(mutex_sem);
    sem_post(full_sem);
  }
  exit(0);
}

static void
consumer(void *arg)
{
  (void)arg;
  for (int i = 1; i <= ITEMS; i++) {
    sem_wait(full_sem);
    sem_wait(mutex_sem);
    int item = buffer[out];
    out = (out + 1) % BUFSIZE;
    printf("Consumer removed %d\n", item);
    sem_post(mutex_sem);
    sem_post(empty_sem);
  }
  exit(0);
}

int
main(void)
{
  mutex_sem = sem_create(1);
  printf("Q2: synchronized shared counter\n");
  for (int i = 0; i < NTHREADS; i++)
    if (clone(counter_worker, (void *)(uint64)i, counter_stacks[i]) < 0) {
      printf("clone failed for counter worker %d\n", i);
      exit(1);
    }
  for (int i = 0; i < NTHREADS; i++)
    join();
  printf("All %d counter threads joined.\n", NTHREADS);
  printf("Synchronized counter = %d (expected %d)\n", counter, NTHREADS * LOOPS);

  empty_sem = sem_create(BUFSIZE);
  full_sem = sem_create(0);
  mutex_sem = sem_create(1);
  printf("Q2: producer-consumer buffer size %d\n", BUFSIZE);
  if (clone(producer, 0, pc_stacks[0]) < 0 ||
      clone(consumer, 0, pc_stacks[1]) < 0) {
    printf("clone failed for producer/consumer\n");
    exit(1);
  }
  join();
  join();
  printf("Producer-consumer complete: %d items processed\n", ITEMS);
  exit(0);
}
