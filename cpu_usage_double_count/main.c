#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	k_thread_runtime_stats_t cpu_cycle0, cpu_cycle1;
	uint32_t real_cycle;
	uint64_t cpu_cycle_delta;

	k_sys_runtime_stats_enable();

        /* z_sched_cpu_usage(): update cpu->usage->total and cpu->usage0 */
	k_thread_runtime_stats_cpu_get(0, &cpu_cycle0);

        /* spin for ~100 ms, record the spin duration to real_cycles */
	real_cycle = k_cycle_get_32();
	k_busy_wait(100 * USEC_PER_MSEC);
	real_cycle = k_cycle_get_32() - real_cycle;

	/* cpu->usage->total += ~100 ms, bug: cpu->usage0 not updated */
	k_thread_runtime_stats_disable(k_current_get());

	/* z_sched_usage_stop(): cpu->usage->total += ~100 ms **AGAIN**, cpu->usage0 = 0 */
        /* it's ok cpu->usage->total stops update for ~50 ms in idle thread */
	/* z_sched_usage_start(): update cpu->usage0 */
	k_sleep(K_MSEC(50));  
        /* z_sched_cpu_usage(): cpu->usage->total += ~0 ms, update cpu->usage0 */
	k_thread_runtime_stats_cpu_get(0, &cpu_cycle1);

	cpu_cycle_delta = cpu_cycle1.total_cycles - cpu_cycle0.total_cycles;

	printk("real cycle: %u\n", real_cycle);
	printk("cpu cycle : %llu\n", cpu_cycle_delta);
	printk("real / cpu: %fx\n", (double)cpu_cycle_delta / real_cycle);

        printk("real / cpu expected ~1.00x, got ~2.00x for the double-count bug\n");

	return 0;
}
