
#ifndef _ARP_H
#define _ARP_H

/*#include <sys/types.h>*/

struct event
{
	long		delta;
	void		(*func) (long);
	long		arg;
	struct event	*next;
};

/*
 * The instruction data structure.
 */
struct bpf_insn
{
	unsigned short	code;
	unsigned char 	jt;
	unsigned char 	jf;
	long	k;
};

struct bpf
{
	struct bpf	*link;		/* next desc in global list */
	struct bpf	*next;		/* next desc for this if */
	struct ifq	recvq;		/* packet input queue */
	struct netif	*nif;		/* the if we are listening to */
	struct bpf_insn *prog;		/* filter program */
	short		proglen;	/* # of insns in the filter program */
	long		tmout;		/* read timeout */
	struct event	evt;		/* timeout event */
	long		rsel;		/* read-selecting process */
	volatile short	flags;		/* what do you think? */
#define BPF_OPEN 0x0001
#define BPF_WAKE 0x0002
	long		hdrlen;		/* MAC header length */
	long		hwtype;		/* hardware type */

	long		in_pkts;	/* # of packets received */
	long		in_drop;	/* # of packets dropped */

	long		s_time;		/* GMT at open() */
	long		s_ticks;	/* HZ200 ticks at open() */
};
/* --------------------------------------------------- */
/*
 * ARP request/reply packet
 */
struct arp_dgram
{
	unsigned short	hwtype;
	unsigned short	prtype;
	unsigned char	hwlen;
	unsigned char	prlen;
	unsigned short	op;
	unsigned char	data[0];
};

#define ARP_SHW(a)	((a)->data)
#define ARP_SPR(a)	((a)->data + (a)->hwlen)
#define ARP_DHW(a)	((a)->data + (a)->hwlen + (a)->prlen)
#define ARP_DPR(a)	((a)->data + 2*(a)->hwlen + (a)->prlen)
#define ARP_LEN(a)	(sizeof(struct arp_dgram) + 2*((a)->hwlen + (a)->prlen))

#endif
