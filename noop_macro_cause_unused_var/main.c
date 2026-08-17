#include <zephyr/kernel.h>
#include <zephyr/kernel/obj_core.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* exercise the enable path that carried the unused `cycles` local */
	k_thread_runtime_stats_disable(k_current_get());
	k_thread_runtime_stats_enable(k_current_get());

	/* z_thread_stats_reset() on the current thread hits the other
	 * sched_cpu_update_usage() call site with an unused `cycles` local
	 */
	k_obj_core_stats_reset(K_OBJ_CORE(k_current_get()));

	printk("ANALYSIS=y ALL=n build: PASS\n");

	return 0;
}
