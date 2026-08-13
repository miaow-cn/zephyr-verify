# stack_canaries_tls

Verifies the `#include` fix in `zephyr/kernel/sys/thread_entry.c`:

`sys_rand_get()` is used to initialize the TLS stack canary
(`__stack_chk_guard`) under `CONFIG_STACK_CANARIES_TLS`, but
`<zephyr/random/random.h>` was included under
`CONFIG_CURRENT_THREAD_USE_TLS` instead. With
`CONFIG_STACK_CANARIES_TLS=y` and `CONFIG_CURRENT_THREAD_USE_TLS=n`
the build failed with an implicit declaration of `sys_rand_get`.

## Build & run

The case needs an arch with `ARCH_HAS_STACK_CANARIES_TLS`
(x86 or riscv); `CURRENT_THREAD_USE_TLS=n` turns off the current-thread
TLS variable:

```
west build -b qemu_x86 verify/stack_canaries_tls
west build -t run
```

Expected: build succeeds and the console prints

```
STACK_CANARIES_TLS with CURRENT_THREAD_USE_TLS=n: PASS
```
