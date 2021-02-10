#ifndef __IOCTL_H__
#define __IOCTL_H__

#include "offset.h"

#define IOCTL_BASE 'W'
#define CPUCTL_GET_OFFSETS _IOR(IOCTL_BASE, 1, struct offsets)
#define CPUCTL_SET_OFFSETS _IOW(IOCTL_BASE, 2, struct offsets)

#endif
