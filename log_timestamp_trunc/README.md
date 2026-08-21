# log_timestamp_trunc

Reproducer for a truncation bug in Zephyr's `timestamp_print()` in
`subsys/logging/log_output.c` (the default `hh:mm:ss.ms,us` format branch,
`LOG_OUTPUT_FORMAT_TIME_TIMESTAMP`).

## Bug

With `CONFIG_LOG_TIMESTAMP_64BIT=y`, `total_seconds` is `uint64_t`, but the
local variables in the time-format branch are declared 32-bit:

```c
uint32_t seconds;
uint32_t hours;
uint32_t mins;

seconds = total_seconds;   /* silently drops the upper 32 bits */
```

Once uptime exceeds 2^32 seconds (~136 years), the printed timestamp wraps
back to zero. The app does not wait that long — it calls the public
`log_output_process()` directly with a fabricated timestamp whose
`total_seconds` is `2^32 + 3661`.

## Fix
Compute hours from total_seconds directly in 64 bits when CONFIG_LOG_TIMESTAMP_64BIT is enabled. Print it with %llu.

## Build & Run

```sh
west build -b qemu_x86 -t run
```

## Results

Before fix:

```
formatted: [01:01:01.000,000]   # 2^32 + 3661 s truncated to 3661 s
```

After fix:

```
formatted: [1193047:29:17.000,000]   # correct total seconds
```
