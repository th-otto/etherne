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

#define buf_alloc	(*NETINFO->_buf_alloc)
#define buf_free	(*NETINFO->_buf_free)
#define buf_reserve	(*NETINFO->_buf_reserve)
#define buf_deref	(*NETINFO->_buf_deref)

#define if_enqueue	(*NETINFO->_if_enqueue)
#define if_dequeue	(*NETINFO->_if_dequeue)
#define if_register	(*NETINFO->_if_register)
#define if_input	(*NETINFO->_if_input)
#define if_flushq	(*NETINFO->_if_flushq)
#define if_getfreeunit	(*NETINFO->_if_getfreeunit)

#define in_chksum	(*NETINFO->_in_chksum)

#define eth_build_hdr	(*NETINFO->_eth_build_hdr)
#define eth_remove_hdr	(*NETINFO->_eth_remove_hdr)

#define bpf_input	(*NETINFO->_bpf_input)
#endif

#include "buf.h"
#include "if.h"

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
