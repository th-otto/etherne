#ifndef _NETINFO_H
#define _NETINFO_H

/*
 * Don't want external declarations that belong to sockdev.xdd
 */
#define NOEXTERNS

#ifndef NETINFO
#define NETINFO netinfo

struct netinfo;
extern struct netinfo *NETINFO;

#define buf_deref	mybuf_deref
#define if_enqueue	myif_enqueue
#define if_dequeue	myif_dequeue
#define	if_register	myif_register
#define if_getfreeunit	myif_getfreeunit
#define eth_build_hdr	myeth_build_hdr
#define bpf_input	mybpf_input
#endif

#include "buf.h"
#include "if.h"

void	mybuf_deref (BUF *, short);
short	myif_enqueue (struct ifq *, BUF *, short);
BUF *	myif_dequeue (struct ifq *);
long	myif_register (struct netif *);
short	myif_getfreeunit (char *);
BUF *	myeth_build_hdr (BUF *, struct netif *, char *, short);
long	mybpf_input (struct netif *, BUF *);

struct netinfo
{
	BUF *	cdecl (*_buf_alloc) (ulong, ulong, short);
	void	cdecl (*_buf_free) (BUF *, short);
	BUF *	cdecl (*_buf_reserve) (BUF *, long, short);
	void	cdecl (*_buf_deref) (BUF *, short);

	short	cdecl (*_if_enqueue) (struct ifq *, BUF *, short);
	BUF *	cdecl (*_if_dequeue) (struct ifq *);
	long	cdecl (*_if_register) (struct netif *);
	short	cdecl (*_if_input) (struct netif *, BUF *, long, short);
	void	cdecl (*_if_flushq) (struct ifq *);

	short	(*_in_chksum) (void *, short);
	short	cdecl (*_if_getfreeunit) (char *);

	BUF *	cdecl (*_eth_build_hdr) (BUF *, struct netif *, char *, short);
	short	cdecl (*_eth_remove_hdr) (BUF *);

	char	*fname;

	long	cdecl (*_bpf_input) (struct netif *, BUF *);

	long	reserved[5];
};

#endif /* _NETINFO_H */
