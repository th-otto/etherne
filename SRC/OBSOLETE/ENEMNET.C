/************************************************************************/
/*																		*/
/* MagiCNet driver for EtherNE NE2000 adapter							*/
/* Generic for ACSI and Cartridge Port interface hardware				*/
/* Copyright 2000-2002 Dr. Thomas Redelberger							*/
/* Use it under the terms of the GNU General Public License				*/
/* (See file COPYING.TXT)												*/
/*																		*/
/* Module to install and activate the port and to interface transmit 	*/
/* to MagiCNet															*/
/* Receive (done from HZ200 interrupts) calls MagiCNet upper layers		*/
/* directly.															*/
/*																		*/
/* Credits:																*/
/*		Based on dummyeth sceleton, Kay Roemer, 1994-12-14.				*/
/*																		*/
/* Tab size 4, developed with Turbo-C ST 2.0							*/
/************************************************************************/
/*
$Id$
 */


/* 4 defines below are provisional */
#define ENOSYS	-1
#define ENOENT	-2
#define ENOMEM	-3
#define DEBUG(a)


/* why is this so complicated ? */

#if 1
/* my version to fix the A2 difference of Turbo C versus Pure C/GNU C */
#	include "mnetinfo.h"	/* must be first to have NOEXTERNS defined! */
#else
#	include "netinfo.h"	/* must be first to have NOEXTERNS defined! */
#endif
#include "buf.h"
#include "if.h"			/* uses struct bpf, defines struct ifq */
#include "new_arp.h"	/* uses struct ifq, defines struct bpf */
#include "ifeth.h"
#include "sockios.h"
#include "file.h"		/* for what? */


/* I use these types */

typedef          char  int8;		/*   Signed  8 bit (char)			*/
typedef unsigned char uint8;		/* Unsigned  8 bit (byte, octet)	*/
typedef          int   int16;		/*   Signed 16 bit (int)			*/
typedef unsigned int  uint16;		/* Unsigned 16 bit (word)			*/
typedef          long  int32;		/*   Signed 32 bit					*/
typedef unsigned long uint32;		/* Unsigned 32 bit (longword)		*/

#include "ne.h"
#include "uti.h"


extern void NEInstInt(void);		/* reference into NEMNET.S to install my HZ200 interrupt */
extern ushort myspl7(void);
extern void myspl(ushort);


/* avoid STDLIB */

#define	memcp6(pd, ps, dummy) \
	{char*d_ = (char*) pd; char*s_ = (char*) ps; \
		*d_++ = *s_++; *d_++ = *s_++; *d_++ = *s_++; \
		*d_++ = *s_++; *d_++ = *s_++; *d_++ = *s_++;		 }




/*
 * Prototypes for our service functions
 */
static long	cdecl ENE_open		(struct netif *);
static long	cdecl ENE_close		(struct netif *);
static long	cdecl ENE_output	(struct netif *, BUF *, char *, short, short);
static long	cdecl ENE_ioctl		(struct netif *, short, long);
static long	      ENE_config	(struct netif *, struct ifopt *);


/*
 * Our interface structure
 */
struct netif if_ENE = {
	"en",		/* name[IF_NAMSIZ] */
	0,			/* unit */
	IFF_BROADCAST,			/* flags */
	0,			/* metric */
	1500,		/* mtu */
	0,			/* timer */
	HWTYPE_ETH,	/* hwtype */

	ETH_ALEN,	/* hwlocal.len */
	{0,0,0,0,0,0},	/* hwlocal.addr */
	ETH_ALEN,	/* hwbrcst.len */
	"\377\377\377\377\377\377",	/* hwbrcst.addr */

	NULL,		/* struct ifaddr* addrlist */

	IF_MAXQ,	/* snd.maxqlen */
	0,			/* snd.qlen */
	0,			/* snd.curr */
	NULL, NULL, NULL, NULL,	/* *qfirst[2], *qlast[2] */

	IF_MAXQ,	/* rcv.maxqlen */
	0,			/* rcv.qlen */
	0,			/* rcv.curr */
	NULL, NULL, NULL, NULL,	/* *qfirst[2], *qlast[2] */

	ENE_open,	/* *open */
	ENE_close,	/* *close */
	ENE_output,	/* *output */
	ENE_ioctl,	/* *ioctl */
	NULL,		/* *timeout */

	NULL,		/* void *data */

	0,			/* ulong in_packets */
	0,			/* ulong in_errors */
	0,			/* ulong out_packets */
	0,			/* ulong out_errors */
	0,			/* ulong collisions */

	NULL,		/* struct netif *next */

	4,			/* short maxpackets */

	NULL,		/* struct bpf *bpf */

	0,0,0,0		/* long reserved[4] */

};


static long driver_init (void);


struct kerinfo *KERNEL;
struct netinfo *NETINFO;

/* init MUST be the first executable code in the TEXT segment */
/* there is no other startup code */


/*
 * Initialization. This is called when the driver is loaded.
 *
 * You should probe for your hardware here, setup the interface
 * structure and register your interface.
 *
 * This function should return 0 on success and != 0 if initialization
 * fails.
 */

static long cdecl init (struct kerinfo *k, struct netinfo *n)
{
	KERNEL = k;
	NETINFO = n;
	
	if (ne_probe1() != 0) return 1;

#if 0		
	/*
	 * Set interface name
	 */
	if_ENE.name[0] = 'e';
	if_ENE.name[1] = 'n';
	if_ENE.name[2] = '\0';
	/*
	 * Alays set to zero
	 */
	if_ENE.metric = 0;
	/*
	 * Initial interface flags, should be IFF_BROADCAST for
	 * Ethernet.
	 */
	if_ENE.flags = IFF_BROADCAST;
	/*
	 * Maximum transmission unit, should be >= 46 and <= 1500 for
	 * Ethernet
	 */
	if_ENE.mtu = 1500;
	/*
	 * Time in ms between calls to (*if_ENE.timeout) ();
	 */
	if_ENE.timer = 0;
	
	/*
	 * Interface hardware type
	 */
	if_ENE.hwtype = HWTYPE_ETH;
	/*
	 * Hardware address length, 6 bytes for Ethernet
	 */
	if_ENE.hwlocal.len =
	if_ENE.hwbrcst.len = ETH_ALEN;
	
	/*
	 * Set interface hardware and broadcast addresses. For real ethernet
	 * drivers you must get them from the hardware of course!
	 */
	memcp6 (if_ENE.hwbrcst.addr, "\377\377\377\377\377\377", ETH_ALEN);
	/*
	 * Set length of send and receive queue. IF_MAXQ is a good value.
	 */
	if_ENE.rcv.maxqlen = IF_MAXQ;
	if_ENE.snd.maxqlen = IF_MAXQ;
	/*
	 * Setup pointers to service functions
	 */
	if_ENE.open		= ENE_open;
	if_ENE.close	= ENE_close;
	if_ENE.output	= ENE_output;
	if_ENE.ioctl	= ENE_ioctl;
	/*
	 * Optional timer function that is called every 200ms.
	 */
	if_ENE.timeout = 0;
	
	/*
	 * Here you could attach some more data your driver may need
	 */
	if_ENE.data = 0;

	/*
	 * Number of packets the hardware can receive in fast succession,
	 * 0 means unlimited.
	 */
	if_ENE.maxpackets = 4;		/* full sized (1500 bytes) */

#endif
	/*
	 * Set interface unit. if_getfreeunit("name") returns a yet
	 * unused unit number for the interface type "name".
	 */
	if_ENE.unit = if_getfreeunit ("en");

	
	memcp6 (if_ENE.hwlocal.addr, DVS.dev_dev_addr, ETH_ALEN);

	/*
	 * Register the interface.
	 */
	if_register (&if_ENE);
	
	/*
	 * NETINFO->fname is a pointer to the drivers file name
	 * (without leading path), eg. "dummy.xif".
	 * NOTE: the file name will be overwritten when you leave the
	 * init function. So if you need it later make a copy!
	 */

	if (NETINFO->fname) {
		prntStr (NETINFO->fname);
		prntStr ("\n\r");
	}	

	/*
	 * And say we are alive...
	 */
	{
		static char message[] = "EtherNE driver v0.1 (en0) MAC: ";
#		define	OFFSUNIT	23	/* offset of the "0" from beginning of message */
		int i;
		char* b = if_ENE.hwlocal.addr;
		message[OFFSUNIT] += if_ENE.unit;
		prntStr (message);
		for (i=0; i<0; i++, b++) {
			prntByte(*b); prntStr(":");
		}
		prntStr("\b\n\r");
	}

	NEInstInt();		/* link ei_interrupt into HZ200 interrupt */

	return 0;
}



/*
 * This gets called when someone makes an 'ifconfig up' on this interface
 * and the interface was down before.
 */
static long cdecl ENE_open (struct netif *nif)
{
	return ne_open();
}

/*
 * Opposite of ENE_open(), is called when 'ifconfig down' on this interface
 * is done and the interface was up before.
 */
static long cdecl ENE_close (struct netif *nif)
{
	return ne_close();
}

/*
 * This routine is responsible for enqueing a packet for later sending.
 * The packet it passed in `buf', the destination hardware address and
 * length in `hwaddr' and `hwlen' and the type of the packet is passed
 * in `pktype'.
 *
 * `hwaddr' is guaranteed to be of type nif->hwtype and `hwlen' is
 * garuanteed to be equal to nif->hwlocal.len.
 *
 * `pktype' is currently one of (definitions in if.h):
 *	PKTYPE_IP for IP packets,
 *	PKTYPE_ARP for ARP packets,
 *	PKTYPE_RARP for reverse ARP packets.
 *
 * These constants are equal to the ethernet protocol types, ie. an
 * Ethernet driver may use them directly without prior conversion to
 * write them into the `proto' field of the ethernet header.
 *
 * If the hardware is currently busy, then you can use the interface
 * output queue (nif->snd) to store the packet for later transmission:
 *	if_enqueue (&nif->snd, buf, buf->info).
 *
 * `buf->info' specifies the packet's delivering priority. if_enqueue()
 * uses it to do some priority queuing on the packets, ie. if you enqueue
 * a high priority packet it may jump over some lower priority packets
 * that were already in the queue (ie that is *no* FIFO queue).
 *
 * You can dequeue a packet later by doing:
 *	buf = if_dequeue (&nif->snd);
 *
 * This will return NULL is no more packets are left in the queue.
 *
 * The buffer handling uses the structure BUF that is defined in buf.h.
 * Basically a BUF looks like this:
 *
 * typedef struct {
 *	long buflen;
 *	char *dstart;
 *	char *dend;
 *	...
 *	char data[0];
 * } BUF;
 *
 * The structure consists of BUF.buflen bytes. Up until BUF.data there are
 * some header fields as shown above. Beginning at BUF.data there are
 * BUF.buflen - sizeof (BUF) bytes (called userspace) used for storing the
 * packet.
 *
 * BUF.dstart must always point to the first byte of the packet contained
 * within the BUF, BUF.dend points to the first byte after the packet.
 *
 * BUF.dstart should be word aligned if you pass the BUF to any MintNet
 * functions! (except for the buf_* functions itself).
 *
 * BUF's are allocated by
 *	nbuf = buf_alloc (space, reserve, mode);
 *
 * where `space' is the size of the userspace of the BUF you need, `reserve'
 * is used to set BUF.dstart = BUF.dend = BUF.data + `reserve' and mode is
 * one of
 *	BUF_NORMAL for calls from kernel space,
 *	BUF_ATOMIC for calls from interrupt handlers.
 *
 * buf_alloc() returns NULL on failure.
 *
 * Usually you need to pre- or postpend some headers to the packet contained
 * in the passed BUF. To make sure there is enough space in the BUF for this
 * use
 *	nbuf = buf_reserve (obuf, reserve, where);
 *
 * where `obuf' is the BUF where you want to reserve some space, `reserve'
 * is the amount of space to reserve and `where' is one of
 *	BUF_RESERVE_START for reserving space before BUF.dstart
 *	BUF_RESERVE_END for reserving space after BUF.dend
 *
 * Note that buf_reserve() returns pointer to a new buffer `nbuf' (possibly
 * != obuf) that is a clone of `obuf' with enough space allocated. `obuf'
 * is no longer existant afterwards.
 *
 * However, if buf_reserve() returns NULL for failure then `obuf' is
 * untouched.
 *
 * buf_reserve() does not modify the BUF.dstart or BUF.dend pointers, it
 * only makes sure you have the space to do so.
 *
 * In the worst case (if the BUF is to small), buf_reserve() allocates a new
 * BUF and copies the old one to the new one (this is when `nbuf' != `obuf').
 *
 * To avoid this you should reserve enough space when calling buf_alloc(), so
 * buf_reserve() does not need to copy. This is what MintNet does with the BUFs
 * passed to the output function, so that copying is never needed. You should
 * do the same for input BUFs, ie allocate the packet as eg.
 *	buf = buf_alloc (nif->mtu+sizeof (eth_hdr)+100, 50, BUF_ATOMIC);
 *
 * Then up to nif->mtu plus the legth of the ethernet header bytes long
 * frames may ne received and there are still 50 bytes after and before
 * the packet.
 *
 * If you have sent the contents of the BUF you should free it by calling
 *	buf_deref (`buf', `mode');
 *
 * where `buf' should be freed and `mode' is one of the modes described for
 * buf_alloc().
 *
 * Functions that can be called from interrupt:
 *	buf_alloc (..., ..., BUF_ATOMIC);
 *	buf_deref (..., BUF_ATOMIC);
 *	if_enqueue ();
 *	if_dequeue ();
 *	if_input ();
 *	eth_remove_hdr ();
 *	addroottimeout (..., ..., 1);
 */


/*
 * we need to put back into the queue, but if_putback is missing from struct netinfo
 */

static short if_putback (struct ifq *q, BUF *buf, short pri)
{
	register ushort sr = myspl7 ();

	if (q->qlen >= q->maxqlen) {
		/* queue full, dropping packet */
		buf_deref (buf, BUF_ATOMIC);

		myspl (sr);
		return ENOMEM;
	} else {
		if ((ushort)pri >= IF_PRIORITIES)
			pri = IF_PRIORITIES-1;

		buf->link3 = q->qfirst[pri];
		q->qfirst[pri] = buf;
		if (!q->qlast[pri])
			q->qlast[pri] = buf;

		q->qlen++;
	}

	myspl (sr);
	return 0;
}



/*
 * this is called from our HZ200 interrupt handler
 */

void ENE_re_xmit(void)
{
	BUF *nbuf;

	if (if_ENE.snd.qlen > 0 ) {
		if ( (nbuf=if_dequeue(&(if_ENE.snd))) == NULL ) return;
		
		if ( ei_start_xmit (nbuf->dstart, nbuf->dend-nbuf->dstart, NULL, 0) == 0 ) {
			if_ENE.out_packets++;
			buf_deref (nbuf, BUF_NORMAL);
		} else {
			if_putback (&(if_ENE.snd), nbuf, nbuf->info);
		}
	}
}


/*
 * 
 */

static long cdecl
ENE_output (struct netif *nif, BUF *buf, char *hwaddr, short hwlen, short pktype)
{
	BUF *nbuf;
	short type;
	short len;

	
	/*
	 * Attach eth header. MintNet provides you with the eth_build_hdr
	 * function that attaches an ethernet header to the packet in
	 * buf. It takes the BUF (buf), the interface (nif), the hardware
	 * address (hwaddr) and the packet type (pktype).
	 *
	 * Returns NULL if the header could not be attached (the passed
	 * buf is thrown away in this case).
	 *
	 * Otherwise a pointer to a new BUF with the packet and attached
	 * header is returned and the old buf pointer is no longer valid.
	 */

	if ( (nbuf = eth_build_hdr (buf, nif, hwaddr, pktype)) == NULL ) {
		nif->out_errors++;
		return ENOMEM;
	}

	
	/*
	 * Here you should either send the packet to the hardware or
	 * enqueue the packet and send the next packet as soon as
	 * the hardware is finished.
	 *
	 * If you are done sending the packet free it with buf_deref().
	 *
	 * Before sending it pass it to the packet filter.
	 */
	if (nif->bpf)
		bpf_input (nif, nbuf);


	len = nbuf->dend - nbuf->dstart;
	if (len < (ETH_HLEN+ETH_MIN_DLEN)) len = ETH_HLEN+ETH_MIN_DLEN;

	if ( ei_start_xmit (nbuf->dstart, len, NULL, 0) == 0 ) {
		nif->out_packets++;
		buf_deref (nbuf, BUF_NORMAL);
	} else {
		if_enqueue (&nif->snd, nbuf, nbuf->info);
	}


	return 0;
}

/*
 * MintNet notifies you of some noteable IOCLT's. Usually you don't
 * need to act on them because MintNet already has done so and only
 * tells you that an ioctl happened.
 *
 * One useful thing might be SIOCGLNKFLAGS and SIOCSLNKFLAGS for setting
 * and getting flags specific to your driver. For an example how to use
 * them look at slip.c
 */
static long cdecl ENE_ioctl (struct netif *nif, short cmd, long arg)
{
	struct ifreq *ifr;
	
	switch (cmd)
	{
		case SIOCSIFNETMASK:
		case SIOCSIFFLAGS:
		case SIOCSIFADDR:
			return 0;
		
		case SIOCSIFMTU:
			/*
			 * Limit MTU to 1500 bytes. MintNet has alraedy set nif->mtu
			 * to the new value, we only limit it here.
			 */
			if (nif->mtu > ETH_MAX_DLEN)
				nif->mtu = ETH_MAX_DLEN;
			return 0;
		
		case SIOCSIFOPT:
			/*
			 * Interface configuration, handled by ENE_config()
			 */
			ifr = (struct ifreq *) arg;
			return ENE_config (nif, ifr->ifru.data);
	}
	
	return ENOSYS;
}

/*
 * Interface configuration via SIOCSIFOPT. The ioctl is passed a
 * struct ifreq *ifr. ifr->ifru.data points to a struct ifopt, which
 * we get as the second argument here.
 *
 * If the user MUST configure some parameters before the interface
 * can run make sure that ENE_open() fails unless all the necessary
 * parameters are set.
 *
 * Return values	meaning
 * ENOSYS		option not supported
 * ENOENT		invalid option value
 * 0			Ok
 */
static long ENE_config (struct netif *nif, struct ifopt *ifo)
{
#if 0

#define STRNCMP(s)	(strncmp ((s), ifo->option, sizeof (ifo->option)))
	
	if (!STRNCMP ("hwaddr")) {
		uchar *cp;
		/*
		 * Set hardware address
		 */
		if (ifo->valtype != IFO_HWADDR)
			return ENOENT;
		memcp6 (nif->hwlocal.addr, ifo->ifou.v_string, ETH_ALEN);
		cp = nif->hwlocal.addr;
		DEBUG (("dummy: hwaddr is %x:%x:%x:%x:%x:%x",
			cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]));

	} else if (!STRNCMP ("braddr"))	{
		uchar *cp;
		/*
		 * Set broadcast address
		 */
		if (ifo->valtype != IFO_HWADDR)
			return ENOENT;
		memcp6 (nif->hwbrcst.addr, ifo->ifou.v_string, ETH_ALEN);
		cp = nif->hwbrcst.addr;
		DEBUG (("dummy: braddr is %x:%x:%x:%x:%x:%x",
			cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]));

	} else if (!STRNCMP ("debug")) {
		/*
		 * turn debuggin on/off
		 */
		if (ifo->valtype != IFO_INT)
			return ENOENT;
		DEBUG (("dummy: debug level is %ld", ifo->ifou.v_long));

	} else if (!STRNCMP ("log")) {
		/*
		 * set log file
		 */
		if (ifo->valtype != IFO_STRING)
			return ENOENT;
		DEBUG (("dummy: log file is %s", ifo->ifou.v_string));
	}
	
	return ENOSYS;
#else
	return 0;
#endif
}


