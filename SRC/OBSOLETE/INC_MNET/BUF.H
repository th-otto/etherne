
#ifndef _BUF_H
#define _BUF_H

#include "config.h"

#define BUF_NORMAL		0
#define BUF_ATOMIC		1

#define BUF_BLOCK_SIZE	(1024*32L)
#define BUF_NSPLIT		7
#define BUF_MAGIC		0x73ec5a13ul

#define BUF_RESERVE_START	1
#define BUF_RESERVE_END		2

#define BUF_LEAD_SPACE(b)	((long)(b)->dstart - (long)(b)->data)
#define BUF_TRAIL_SPACE(b)	((long)(b) + (b)->buflen - (long)(b)->dend)

typedef struct buf BUF;
struct buf
{
	ulong	buflen;		/* buffer len, including header */
	char	*dstart;	/* start of data */
	char	*dend;		/* end of data */
	BUF		*next;		/* next message */
	BUF		*prev;		/* previous message */
	BUF		*link3;		/* another next pointer */
	short	links;		/* usage counter */
	long	info;		/* aux info */

	BUF		*_n;		/* next buf in memory */
	BUF		*_p;		/* previous buf memory */
	BUF		*_nfree;	/* next free buf of same size */
	BUF		*_pfree;	/* previous free buf of same size */
	char	data[0];
};

#ifndef NOEXTERNS
extern long buf_init (void);
extern BUF * cdecl buf_alloc (unsigned long, unsigned long, short);
extern void	cdecl buf_free (BUF *, short);
extern BUF * cdecl buf_reserve (BUF *, long, short);
extern void	cdecl buf_deref (BUF *, short);
extern BUF	*buf_clone (BUF *, short);
#endif


#if	0	/* code in header files is quite unusual (this allows us to do without init.s) ThR */
static void	buf_ref (BUF *);

/* static inline void */

static void
buf_ref (BUF *buf)
{
	++buf->links;
}
#endif

#endif /* _BUF_H */
