/************************************************************************/
/*																		*/
/*	Helper functions for assembler programming and debugging			*/
/*	Copyright 2002 Dr. Thomas Redelberger								*/
/*	Use it under the terms of the GNU General Public License			*/
/*	(See file COPYING.TXT)												*/
/*																		*/
/* Software interface file												*/
/*																		*/
/* Tabsize 4, developed with Turbo-C ST 2.0								*/
/*																		*/
/************************************************************************/

#define myPling()	prntStr("\7")


/**** entry points in UTI.S ****/

/* printing functions via the BIOS concole */

extern	void	cdecl	prntStr(const char* string);
extern	void	cdecl	prntLong(uint32);
extern	void	cdecl	prntWord(uint16);
extern	void	cdecl	prntByte(uint8);
extern	void	cdecl	prntSR(void);

