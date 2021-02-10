#ifndef __OFFSET_H__
#define __OFFSET_H__

#include <asm-generic/int-ll64.h>

#define MSR_VOLTAGE_CTL 0x150

#define CPU_CORE_PLANE 0
#define IGPU_PLANE 1
#define CPU_CACHE_PLANE 2
#define SYSTEM_AGENT_PLANE 3
#define ANALOG_IO_PLANE 4
#define DIGITAL_IO_PLANE 5
#define PLANES 6

#define UPDATE_CPU_CORE 0x01
#define UPDATE_IGPU 0x02
#define UPDATE_CPU_CACHE 0x04
#define UPDATE_SYSTEM_AGENT 0x08
#define UPDATE_ANALOG_IO 0x10
#define UPDATE_DIGITAL_IO 0x20

struct offsets {
    unsigned int what;
    int values[PLANES];
};

void cpuctl_get_offsets(struct offsets *offsets);
long cpuctl_set_offsets(struct offsets *offsets);

#endif
