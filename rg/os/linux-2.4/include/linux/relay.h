#ifndef _RELAY_H_
#define _RELAY_H_

/* Relay ioctl struct for adding/removing device */
typedef struct mcast_relay_req_t {
    int is_forward; /* all incoming multicast/IGMP from this device will be
		     * forwarded */
    char dev_name[IFNAMSIZ]; /* The interface name */
} mcast_relay_req_t;

/* Disable/Enabele relay */
#define MCAST_RELAY_CTRL _IOW(RG_IOCTL_PREFIX_MCAST_RELAY, 0, int)

/* Add interface */
#define MCAST_RELAY_IF_ADD \
    _IOW(RG_IOCTL_PREFIX_MCAST_RELAY, 1, mcast_relay_req_t)

/* remove interface */
#define MCAST_RELAY_IF_DEL \
    _IOW(RG_IOCTL_PREFIX_MCAST_RELAY, 2, mcast_relay_req_t)

#endif
