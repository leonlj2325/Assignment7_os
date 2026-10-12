#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define NTHREADS 3
#define LOOPS 100000

static volatile int counter = 0;
static char stacks[NTHREADS][PGSIZE] __attribute__((aligned(PGSIZE)));

static void
worker(void *arg)
{
  (void)arg;
  for (int i = 0; i < LOOPS; i++)
    counter++;
  exit(0);
}

int
main(void)
{
  printf("Q1: three threads, %d increments each (no lock)\n", LOOPS);
  for (int i = 0; i < NTHREADS; i++) {
    if (clone(worker, 0, stacks[i]) < 0) {
      printf("clone failed\n");
      exit(1);
    }
  }
  for (int i = 0; i < NTHREADS; i++) {
    int tid = join();
    printf("Joined thread %d\n", tid);
  }
  printf("Final counter = %d (ideal: %d; no lock is used)\n", counter, NTHREADS * LOOPS);
  exit(0);
}
