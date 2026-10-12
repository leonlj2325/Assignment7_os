# OS Lab — Assignment 7

**Leon Joel | 2403MC04**

This repository contains my solutions for OS Lab Assignment 7 using xv6-RISC-V. Each question has its own folder with the source code and an `output` folder for screenshots.

## Questions

- **Q1 – Threads:** Tests `clone()` and `join()` using three threads that increment a shared counter.
- **Q2 – Semaphores:** Tests a synchronized counter and a producer-consumer problem with a buffer of size 5.
- **Q3 – Deadlocks:** Covers deadlock detection, Banker's algorithm, and circular wait.
- **Q4 – Priority Scheduling:** Tests priority-based scheduling with four child processes. The kernel also includes priority aging.

## Running the tests

Build and start xv6 from the source directory:

```sh
cd xv6-riscv
make clean
make
make qemu CPUS=2
```

Run the programs one at a time from the xv6 shell:

```text
threadtest
synctest
deadlock
priotest
```

Each question folder contains its test source and an `output` folder. The Q1 screenshot is included; the Q2–Q4 screenshots can be added after running those tests.

## Notes

- Q1 uses no lock around the shared counter, so the final value may vary between runs.
- For Q4, check the kernel output for `aging:` messages to confirm that priority aging is taking effect.