*********************************************************************************
* Bus access macros for ST/TT ACSI bus for register base hardware		*
* Version for my ACSI-NE2000 interface hardware in fast mode for newer		*
* NE2000 clones									*
*										*
*	Copyright 2002 Dr. Thomas Redelberger					*
*	Use it under the terms of the GNU General Public License		*
*	(See file COPYING.TXT)							*
*										*
* Tabsize 8, developed with DEVPAC assembler 2.0.				*
*										*
*********************************************************************************

*
* manifest constants
*
	ACSI_SLOW_ACCESS equ 0

	.INCLUDE "busenea.i"
