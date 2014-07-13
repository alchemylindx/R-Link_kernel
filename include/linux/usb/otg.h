/* USB OTG (On The Go) defines */
/*
 *
 * These APIs may be used between USB controllers.  USB device drivers
 * (for either host or peripheral roles) don't use these calls; they
 * continue to use just usb_device and usb_gadget.
 */

#ifndef __LINUX_USB_OTG_H
#define __LINUX_USB_OTG_H

#include <linux/notifier.h>

/* OTG defines lots of enumeration states before device reset */
enum usb_otg_state {
	OTG_STATE_UNDEFINED = 0,

	/* single-role peripheral, and dual-role default-b */
	OTG_STATE_B_IDLE,
	OTG_STATE_B_SRP_INIT,
	OTG_STATE_B_PERIPHERAL,

	/* extra dual-role default-b states */
	OTG_STATE_B_WAIT_ACON,
	OTG_STATE_B_HOST,

	/* dual-role default-a */
	OTG_STATE_A_IDLE,
	OTG_STATE_A_WAIT_VRISE,
	OTG_STATE_A_WAIT_BCON,
	OTG_STATE_A_HOST,
	OTG_STATE_A_SUSPEND,
	OTG_STATE_A_PERIPHERAL,
	OTG_STATE_A_WAIT_VFALL,
	OTG_STATE_A_VBUS_ERR,
};

enum usb_xceiv_events {
	USB_EVENT_NONE,         /* no events or cable disconnected */
	USB_EVENT_VBUS,         /* vbus valid event */
	USB_EVENT_ID,           /* id was grounded */
	USB_EVENT_CHARGER,      /* usb dedicated charger */
	USB_EVENT_ENUMERATED,   /* gadget driver enumerated */
};

/*
 * the otg driver needs to interact with both device side and host side
 * usb controllers.  it decides which controller is active at a given
 * moment, using the transceiver, ID signal, HNP and sometimes static
 * configuration information (including "board isn't wired for otg").
 */
struct otg_transceiver {
	struct device		*dev;
	const char		*label;

	u8			default_a;
	enum usb_otg_state	state;

	struct usb_bus		*host;
	struct usb_gadget	*gadget;

	/* for notification of usb_xceiv_events */
	struct blocking_notifier_head   notifier;

	/* to pass extra port status to the root hub */
	u16			port_status;
	u16			port_change;

	/* bind/unbind the host controller */
	int	(*set_host)(struct otg_transceiver *otg,
				struct usb_bus *host);

	/* bind/unbind the peripheral controller */
	int	(*set_peripheral)(struct otg_transceiver *otg,
				struct usb_gadget *gadget);

	/* effective for B devices, ignored for A-peripheral */
	int	(*set_power)(struct otg_transceiver *otg,
				unsigned mA);

	/* for non-OTG B devices: set transceiver into suspend mode */
	int	(*set_suspend)(struct otg_transceiver *otg,
				int suspend);

	/* for B devices only:  start session with A-Host */
	int	(*start_srp)(struct otg_transceiver *otg);

	/* start or continue HNP role switch */
	int	(*start_hnp)(struct otg_transceiver *otg);

	/* ask the link to save internal context */
	void    (*link_save_context)(struct otg_transceiver *otg);

	/* ask the link to restore internal context */
	void    (*link_restore_context)(struct otg_transceiver *otg);

	/* ask the link to always be in an active state */
	void    (*link_force_active)(int enable);

	/* pointer to the link driver */
	void    *link;

};


/* for board-specific init logic */
extern int otg_set_transceiver(struct otg_transceiver *);

#if defined(CONFIG_NOP_USB_XCEIV) || (defined(CONFIG_NOP_USB_XCEIV_MODULE) && defined(MODULE))
/* sometimes transceivers are accessed only through e.g. ULPI */
extern void usb_nop_xceiv_register(void);
extern void usb_nop_xceiv_unregister(void);
#else
static inline void usb_nop_xceiv_register(void)
{
}

static inline void usb_nop_xceiv_unregister(void)
{
}
#endif


/* for usb host and peripheral controller drivers */
extern struct otg_transceiver *otg_get_transceiver(void);
extern void otg_put_transceiver(struct otg_transceiver *);

/* Context: can sleep */
static inline int
otg_start_hnp(struct otg_transceiver *otg)
{
	return otg->start_hnp(otg);
}


/* for HCDs */
static inline int
otg_set_host(struct otg_transceiver *otg, struct usb_bus *host)
{
	return otg->set_host(otg, host);
}


/* for usb peripheral controller drivers */

/* Context: can sleep */
static inline int
otg_set_peripheral(struct otg_transceiver *otg, struct usb_gadget *periph)
{
	return otg->set_peripheral(otg, periph);
}

static inline int
otg_set_power(struct otg_transceiver *otg, unsigned mA)
{
	return otg->set_power(otg, mA);
}

/* Context: can sleep */
static inline int
otg_set_suspend(struct otg_transceiver *otg, int suspend)
{
	if (otg->set_suspend != NULL)
		return otg->set_suspend(otg, suspend);
	else
		return 0;
}

static inline int
otg_start_srp(struct otg_transceiver *otg)
{
	return otg->start_srp(otg);
}

/* notifiers */
static inline int
otg_register_notifier(struct otg_transceiver *otg, struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&otg->notifier, nb);
}

static inline void
otg_unregister_notifier(struct otg_transceiver *otg, struct notifier_block *nb)
{
	blocking_notifier_chain_unregister(&otg->notifier, nb);
}

/* for OTG controller drivers (and maybe other stuff) */
extern int usb_bus_start_enum(struct usb_bus *bus, unsigned port_num);

#endif /* __LINUX_USB_OTG_H */
