# noop_macro_cause_unused_var

Build reproducer for unused-variable warnings at
`sched_cpu_update_usage()` call sites in `zephyr/kernel/usage.c`.

## Bug

With `CONFIG_SCHED_THREAD_USAGE_ALL=n`, `sched_cpu_update_usage()` is a
no-op macro. Two functions declare a `cycles` local that is only passed
to that macro and is otherwise never read:

- `k_thread_runtime_stats_enable()` (needs `CONFIG_SCHED_THREAD_USAGE_ANALYSIS=y`):

```c
uint32_t now = usage_now();
uint32_t cycles = now - cpu->usage0;

sched_cpu_update_usage(cpu, cycles);   /* expands to do { } while (0) */
```

- `z_thread_stats_reset()` (needs `CONFIG_OBJ_CORE_STATS_THREAD=y`, not
  gated on ANALYSIS):

```c
uint32_t now = usage_now();
uint32_t cycles = now - _current_cpu->usage0;

sched_cpu_update_usage(_current_cpu, cycles);
```

The build fails with `-Wunused-but-set-variable` when
`CONFIG_COMPILER_WARNINGS_AS_ERRORS=y`.

## Fix

Pass `now - cpu->usage0` straight to the macro:

```c
uint32_t now = usage_now();

sched_cpu_update_usage(cpu, now - cpu->usage0);
```

## Build & run

```
west build -b qemu_x86
west build -t run
```

Expected: build succeeds and the console prints

```
ANALYSIS=y ALL=n build: PASS
```
