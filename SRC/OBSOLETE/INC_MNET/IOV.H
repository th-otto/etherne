/*
 *	Definitions to handle io vectors.
 *
 *	11/13/93, kay roemer.
 */

#ifndef _IOV_H
#define _IOV_H

/* Maximum number of elements in an io vector */
#define IOV_MAX		16

typedef	struct iovec
{
	void	*iov_base;
	long	iov_len;
}IOVEC;

#ifndef NOEXTERNS
extern long iov2buf_cpy (char *, long, const struct iovec *, short, long);
extern long buf2iov_cpy (char *, long, const struct iovec *, short, long);
static long iov_size (const struct iovec *, short);

/* static inline long */
static long
iov_size (const struct iovec *iov, short n)
{
	register long size;

	if (n <= 0 || n > IOV_MAX) return -1;
	for (size = 0; n; ++iov, --n)
	{
		if (iov->iov_len < 0)
			return -1;
		size += iov->iov_len;
	}
	return size;
}
#endif

#endif /* _IOV_H */
