#include "kernel/types.h"
#include "user/user.h"

#define N 3
#define M 2

static int allocation[N][M] = {{1, 0}, {0, 1}, {0, 0}};
static int request[N][M] = {{0, 1}, {1, 0}, {1, 1}};
static int available[M] = {0, 0};

static void
print_state(int alloc[N][M], int req[N][M], int avail[M])
{
  printf("Available: [%d, %d]\n", avail[0], avail[1]);
  for (int i = 0; i < N; i++)
    printf("P%d Allocation=[%d,%d] Request=[%d,%d]\n", i,
      alloc[i][0], alloc[i][1], req[i][0], req[i][1]);
}

static int
detect_deadlock(int alloc[N][M], int req[N][M], int avail[M], int *deadlocked)
{
  int work[M] = {avail[0], avail[1]};
  int finish[N] = {0, 0, 0};
  int count = 0, progress;
  do {
    progress = 0;
    for (int i = 0; i < N; i++) {
      if (finish[i]) continue;
      if (req[i][0] <= work[0] && req[i][1] <= work[1]) {
        work[0] += alloc[i][0];
        work[1] += alloc[i][1];
        finish[i] = 1;
        progress = 1;
      }
    }
  } while (progress);
  for (int i = 0; i < N; i++)
    if (!finish[i]) deadlocked[count++] = i;
  return count;
}

static int
is_safe_state(int alloc[N][M], int max[N][M], int avail[M])
{
  int work[M] = {avail[0], avail[1]};
  int finish[N] = {0, 0, 0};
  int need[N][M];
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++) need[i][j] = max[i][j] - alloc[i][j];
  for (int step = 0; step < N; step++) {
    int found = 0;
    for (int i = 0; i < N; i++) {
      if (!finish[i] && need[i][0] <= work[0] && need[i][1] <= work[1]) {
        work[0] += alloc[i][0]; work[1] += alloc[i][1];
        finish[i] = 1; found = 1;
      }
    }
    if (!found) break;
  }
  for (int i = 0; i < N; i++) if (!finish[i]) return 0;
  return 1;
}

static void
real_deadlock_demo(void)
{
  int p1[2], p2[2];
  if (pipe(p1) < 0 || pipe(p2) < 0) {
    printf("pipe setup failed\n"); return;
  }
  int a = fork();
  if (a == 0) {
    close(p1[0]); close(p2[1]);
    char c; read(p2[0], &c, 1); exit(0);
  }
  int b = fork();
  if (b == 0) {
    close(p2[0]); close(p1[1]);
    char c; read(p1[0], &c, 1); exit(0);
  }
  close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]);
  printf("Circular wait started; terminating the blocked demo processes.\n");
  for (volatile int i = 0; i < 2000000; i++) ;
  kill(a); kill(b); wait(0); wait(0);
  printf("Deadlock demo cleaned up.\n");
}

int
main(void)
{
  int dead[N];
  printf("Q3: deadlock detection matrix\n");
  print_state(allocation, request, available);
  int n = detect_deadlock(allocation, request, available, dead);
  if (n) { printf("Deadlocked processes:"); for (int i=0;i<n;i++) printf(" P%d", dead[i]); printf("\n"); }
  else printf("No deadlock detected\n");

  int safe_alloc[N][M] = {{1,0},{0,1},{1,0}};
  int safe_req[N][M] = {{0,0},{0,0},{0,0}};
  int safe_avail[M] = {0,0};
  printf("Safe matrix test: ");
  n = detect_deadlock(safe_alloc, safe_req, safe_avail, dead);
  printf(n ? "deadlock\n" : "no deadlock\n");

  int bank_alloc[N][M] = {{0,0},{0,0},{0,1}};
  int bank_max[N][M] = {{0,0},{0,2},{0,2}};
  int bank_avail[M] = {0,1};
  printf("Banker's current state: %s\n", is_safe_state(bank_alloc, bank_max, bank_avail) ? "SAFE" : "UNSAFE");
  printf("Available=[%d,%d]\n", bank_avail[0], bank_avail[1]);
  for (int i=0;i<N;i++)
    printf("P%d Allocation=[%d,%d] Need=[%d,%d]\n", i,
      bank_alloc[i][0], bank_alloc[i][1],
      bank_max[i][0]-bank_alloc[i][0], bank_max[i][1]-bank_alloc[i][1]);
  // Hypothetically grant one unit of resource 1 to P1.
  bank_alloc[1][1]++;
  bank_avail[1]--;
  printf("After granting request P1=[0,1]: %s\n", is_safe_state(bank_alloc, bank_max, bank_avail) ? "SAFE" : "UNSAFE");
  printf("Available=[%d,%d]\n", bank_avail[0], bank_avail[1]);
  for (int i=0;i<N;i++)
    printf("P%d Allocation=[%d,%d] Need=[%d,%d]\n", i,
      bank_alloc[i][0], bank_alloc[i][1],
      bank_max[i][0]-bank_alloc[i][0], bank_max[i][1]-bank_alloc[i][1]);
  real_deadlock_demo();
  exit(0);
}
