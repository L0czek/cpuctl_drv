#include "offset.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");

static u32 get_offset(u64 plane) {
	u64 msr_value = 
		(1ULL << 63) |
		((plane & 0xf) << 40) |
		(1ULL << 36) |
		(0ULL << 32) |
		0ULL;
	
	u32 low = msr_value & 0xffffffff;
	u32 high = msr_value >> 32;
	printk(KERN_INFO "cpuctl: Reading plane %lld\n", plane);
	__wrmsr(MSR_VOLTAGE_CTL, low, high);
	return __rdmsr(MSR_VOLTAGE_CTL) & 0xffffffff;
}

static void set_offset(u64 plane, u64 offset) {
	u64 msr_value = 
		(1ULL << 63) |
		((plane & 0xf) << 40) |
		(1ULL << 36) |
		(1ULL << 32) |
		(offset & 0xffffffff);

	u32 low = msr_value & 0xffffffff;
	u32 high = msr_value >> 32;
	printk(KERN_INFO "cpuctl: Setting plane %lld to %llX\n", plane, offset);
	__wrmsr(MSR_VOLTAGE_CTL, low, high);
}

void cpuctl_get_offsets(struct offsets* offsets) {
	u64 i;
	printk(KERN_INFO "cpuctl: offset->what %d\n", offsets->what);
	for (i=0; i < PLANES; ++i) {
		if ((1ULL << i) & offsets->what) {
			offsets->values[i] = get_offset(i);
		}
	}
}

long cpuctl_set_offsets(struct offsets* offsets) {
	u64 i;
	for (i=0; i < PLANES; ++i) {
		if ((1ULL << i) & offsets->what) {
			set_offset(i, offsets->values[i]);
			if (get_offset(i) != offsets->values[i]) {
				printk(KERN_ERR "cpuctl error setting offset for plane: %lld\n", i);
				return -EFAULT;
			}
		}
	}
	return 0;
}
