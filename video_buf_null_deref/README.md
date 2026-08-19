# video_buf_null_deref

Reproducer for a NULL pointer dereference in `video_buffer_release()`

## Bug

In `zephyr/subsys/video/buffer.c`, the combined check

```c
if (vbuf == NULL || vbuf->index >= ARRAY_SIZE(video_buf)) {
	LOG_ERR("Invalid buffer index: %u", vbuf->index);
	return -EINVAL;
}
```

short-circuits into the error branch when `vbuf` is NULL, but the `LOG_ERR` then unconditionally dereferences `vbuf->index` — a NULL pointer dereference.

## Fix

Split the NULL check from the index bounds check:

```c
if (vbuf == NULL) {
	LOG_ERR("Invalid buffer");
	return -EINVAL;
}
```

## Build & run

Dereferencing address 0 faults reliably on native_sim (SIGSEGV); on some targets address 0 is mapped to real memory and may not fault:

```sh
west build -b native_sim verify/video_buf_null_deref
west build -t run
```

Expected after fix:

```
*** Booting Zephyr OS build d698ef5a7277 ***
[00:00:00.000,000] <err> video_buffer: Invalid buffer
video_buffer_release(NULL) returned -22
expected -22 without fault: PASS
```

Before fix: the app crashes inside `video_buffer_release()` with a NULL dereference (SIGSEGV on native_sim) before printing anything.
