
#include "netinfo.h"
#include "new_arp.h"

void	mybuf_deref (BUF *a, short b)
{
	buf_deref (a, b);
}

short	myif_enqueue (struct ifq * a, BUF * b, short c)
{
	return	if_enqueue (a, b, c);
}

BUF *	myif_dequeue (struct ifq * a)
{
	return	if_dequeue (a);
}

long	myif_register (struct netif * a)
{
	return	if_register (a);
}

short	myif_getfreeunit (char * a)
{
	return	if_getfreeunit (a);
}

BUF *	myeth_build_hdr (BUF * a, struct netif * b, char * c, short d)
{
	return	eth_build_hdr (a, b, c, d);
}

long	mybpf_input (struct netif * a, BUF * b)
{
	return	bpf_input (a, b);
}

