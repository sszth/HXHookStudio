#include<ntddk.h>

#define  IOCTL_LOAD_PROCESS_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x801,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_MONITOR_ON \
    CTL_CODE( FILE_DEVICE_NETWORK,0x802,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_MONITOR_OFF \
    CTL_CODE( FILE_DEVICE_NETWORK,0x803,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_BLOCK_ALL \
    CTL_CODE( FILE_DEVICE_NETWORK,0x804,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_LOAD_IP_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x805,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_UNLOAD_PROCESS_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x806,METHOD_NEITHER,FILE_ANY_ACCESS )

#define  IOCTL_UNLOAD_IP_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x807,METHOD_NEITHER,FILE_ANY_ACCESS )

#define IOCTL_LOAD_DNS_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x808,METHOD_NEITHER,FILE_ANY_ACCESS )

#define IOCTL_UNLOAD_DNS_CONFIG \
    CTL_CODE( FILE_DEVICE_NETWORK,0x809,METHOD_NEITHER,FILE_ANY_ACCESS )                      