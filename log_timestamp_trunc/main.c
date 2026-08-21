#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_output.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/minmax.h>
#include <string.h>

static char capture[128];
static size_t capture_len;

static int capture_func(uint8_t *buf, size_t size, void *ctx)
{
	ARG_UNUSED(ctx);

	size_t n = min(size, sizeof(capture) - 1 - capture_len);

	memcpy(capture + capture_len, buf, n);
	capture_len += n;
	capture[capture_len] = '\0';

	return size;
}

static uint8_t out_buf[64];
LOG_OUTPUT_DEFINE(test_output, capture_func, out_buf, sizeof(out_buf));

int main(void)
{
	/* 1 MHz timestamp frequency: timestamps are in microseconds. */
	log_output_timestamp_freq_set(1000000U);

	/*
	 * Feed a timestamp whose total_seconds = 2^32 + 3661, i.e. 1h 1m 1s
	 * past the uint32_t wrap point (~136 years of uptime).
	 *
	 * timestamp_print() declares "uint32_t seconds" even when
	 * CONFIG_LOG_TIMESTAMP_64BIT=y makes total_seconds 64-bit wide,
	 * so "seconds = total_seconds" silently truncates back to 3661.
	 */
	uint64_t now_seconds = 0x100000000ULL + 3661ULL;
	log_timestamp_t ts = (log_timestamp_t)(now_seconds * 1000000ULL);

	log_output_process(&test_output, ts, NULL, NULL, NULL, 0,
			   LOG_LEVEL_INF, NULL, NULL, 0,
			   LOG_OUTPUT_FLAG_TIMESTAMP |
			   LOG_OUTPUT_FLAG_FORMAT_TIMESTAMP);

	printk("formatted: %s", capture);
	printk("expected:  [1193047:29:17.000,000]\n");
	printk("buggy:     [01:01:01.000,000]\n");

	return 0;
}
