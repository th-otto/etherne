

		XDEF	myif_register
		XDEF	myif_getfreeunit
		XDEF	mybpf_input
		XDEF	myeth_build_hdr
		XDEF	myif_dequeue
		XDEF	myif_enqueue
		XDEF	mybuf_deref

		XREF	netinfo


		SECTION TEXT

mybuf_deref	move.l	a2,-(sp)
		move	d0,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	12(a1),a0
		jsr	(a0)
		addq	#6,sp
		move.l	(sp)+,a2
		rts

myif_enqueue	move.l	a2,-(sp)
		move	d0,-(sp)
		move.l	a1,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	16(a1),a0
		jsr	(a0)
		lea	10(sp),sp
		move.l	(sp)+,a2
		rts

myif_dequeue	move.l	a2,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	20(a1),a0
		jsr	(a0)
		addq	#4,sp
		movea.l	d0,a0
		move.l	(sp)+,a2
		rts

myif_register	move.l	a2,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	24(a1),a0
		jsr	(a0)
		addq	#4,sp
		move.l	(sp)+,a2
		rts

myif_getfreeunit	move.l	a2,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	40(a1),a0
		jsr	(a0)
		addq	#4,sp
		move.l	(sp)+,a2
		rts

myeth_build_hdr	move.l	a2,-(sp)
		move	d0,-(sp)
		move.l	10(sp),-(sp)
		move.l	a1,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	44(a1),a0
		jsr	(a0)
		lea	14(sp),sp
		movea.l	d0,a0
		move.l	(sp)+,a2
		rts


mybpf_input	move.l	a2,-(sp)
		move.l	a1,-(sp)
		move.l	a0,-(sp)
		movea.l	netinfo,a1
		movea.l	56(a1),a0
		jsr	(a0)
		addq	#8,sp
		move.l	(sp)+,a2
		rts
