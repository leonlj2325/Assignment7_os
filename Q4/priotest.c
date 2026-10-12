#include "kernel/types.h"
#include "user/user.h"

#define NCHILD 4
#define WORK 90000000

int
main(void)
{
  int priorities[NCHILD] = {1, 5, 10, 20};
  int pids[NCHILD];

  printf("Q4: priority scheduler test (1=highest, 20=lowest)\n");
  for (int i = 0; i < NCHILD; i++) {
    int pid = fork();
    if (pid == 0) {
      pause(2);
      volatile uint64 x = 0;
      for (int j = 0; j < WORK; j++)
        x += (uint64)j;
      (void)x;
      exit(0);
    }
    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    pids[i] = pid;
    if (set_priority(pid, priorities[i]) < 0) {
      printf("Could not set priority for pid=%d\n", pid);
      exit(1);
    }
    printf("Started pid=%d with priority %d\n", pid, priorities[i]);
  }

  for (int i = 0; i < NCHILD; i++) {
    int done = wait(0);
    for (int j = 0; j < NCHILD; j++) {
      if (pids[j] == done) {
        printf("Child pid=%d (priority %d) completed\n", done, priorities[j]);
        break;
      }
    }
  }
  printf("All children completed. Check the aging messages from the kernel.\n");
  exit(0);
}
