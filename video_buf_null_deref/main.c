#include <zephyr/kernel.h>
#include <zephyr/video/video.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	int ret;

	/*
	 * Buggy code in video_buffer_release():
	 *
	 *	if (vbuf == NULL || vbuf->index >= ARRAY_SIZE(video_buf)) {
	 *		LOG_ERR("Invalid buffer index: %u", vbuf->index);
	 *
	 * short-circuits into the error branch, then the LOG_ERR
	 * unconditionally dereferences vbuf->index -> NULL deref.
	 *
	 * With the fix (NULL check split from the index bounds check),
	 * the call returns -EINVAL cleanly.
	 */
	ret = video_buffer_release(NULL);

	printk("video_buffer_release(NULL) returned %d\n", ret);
	printk("expected %d without fault: %s\n", -EINVAL,
	       ret == -EINVAL ? "PASS" : "FAIL");

	return ret == -EINVAL ? 0 : 1;
}
