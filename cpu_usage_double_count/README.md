# cpu_usage_double_count

Reproducer for a double-counting bug in Zephyr's `k_thread_runtime_stats_disable()`.

## Bug

When disabling runtime stats for the current thread, the function adds the elapsed window to the CPU stats via `sched_cpu_update_usage()` but forgets to advance `cpu->usage0`. The next context switch's `z_sched_usage_stop()` then recomputes the same window from the stale `usage0` and adds it again to `cpu->usage->total`.

Note: the thread's own `track_usage` flag correctly blocks the per-thread path, but the CPU-stats path is gated by a separate `cpu->usage->track_usage` which stays enabled — so the CPU total is what gets double-counted.

## Fix

Add the missing line in `k_thread_runtime_stats_disable()`:

```c
cpu->usage0 = usage_now();
```

## Build & Run

```sh
west build -b native_sim/native/64
./build/zephyr/zephyr.exe
```

## Results

Before fix:

```
real cycle: 100000
cpu cycle : 199998
real / cpu: 1.999980x   # double-counted
```

After fix:

```
real cycle: 100000
cpu cycle : 99999
real / cpu: 0.999990x   # correct
```
