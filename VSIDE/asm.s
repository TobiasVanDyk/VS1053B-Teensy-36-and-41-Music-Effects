#ifdef VS1063
#include <vs1063.h>
#else
#include <vs1053.h>
#endif

	.sect code,int
	.org 0x23
	jmpi mymodu_int,(i6)+1

	.sect code,mymodu_int
	.export mymodu_int	//TODO: mono mode!
mymodu_int:
	stx MR0,(I6)+1	; sty i7,(i6)
	stx lr1,(i6)+1	; sty lr0,(i6)
	stx D0,(I6)+1	; sty I1,(i6)
	stx i5,(i6)+1	; sty i0,(i6)
	stx C2,(I6)+1	; sty D2,(i6)
	stx A2,(I6)+1	; sty B2,(i6)
	stx A0,(I6)+1	; sty A1,(i6)
	add null,p,a
	stx A0,(I6)+1	; sty A1,(i6)
	stx C0,(I6)+1	; sty B0,(i6)
	sty I4,(i6)

	ldc INT_GLOB_ENA,i7
	stx i7,(i7)	// enable interrupts while we process..

	ldc SCI_AICTRL1,i1
	ldx (i1),a0
	add a0,null,a0
	ldc DECIM_DATA_LEFT,I1
	jzs $3		// perform AGC (separate channels)
	ldc MR_INT|MR_SAT,mr0

	//DC removal:
	// leftout = leftin - (moduOffset[0]>>16);
	// moduOffset[0] += leftout;
	// rightout = rightin - (moduOffset[1]>>16);
	// moduOffset[1] += rightout;

	.import _adcControl
	ldc _adcControl+4,i4
	ldx (i1)+1,d0
	sub null,ones,b0	; ldx (i4)-1,a1	//4
	sub d0,a1,c0
	mulss b0,c0	; ldx (i4),a0	//3
	add a,p,a	; ldx (i1),d0
	stx a0,(i4)+1	//3
	stx a1,(i4)+5	//4
	ldx (i4)-1,a1	//9
	sub d0,a1,d0	; ldx (i4)+1,a0	//8
	mulss b0,d0
	add a,p,a
	stx a1,(i4)-1	//9
	and a,null,a	; stx a0,(i4)+2	//8

	// perform gain control
	ldc SCI_AICTRL1,i1
	ldx (i1),b0
	mulsu c0,b0
	macsu d0,b0,a
	ldc 6,b0
	ashl a,b0,a	// 1024 = 1.0x
	rnd a,c0
	add null,p,a
	ashl a,b0,a
	j $10
	rnd a,d0	; ldx (i4),b0	//10 adcMode

	// with AGC
$3:	ldc SCI_AICTRL2,i4
	ldx (i4),a1	//max gain
	//ldc 32768,a1
	ldx (i1),a0	; ldy (i6)+1,null	//data left
	.import _agcConsts
	ldc _agcConsts,i1
#ifdef VS1063
	.import _AGCKeepDC
	call _AGCKeepDC
#else
	.import _AGC
	call _AGC
#endif
	ldc _adcControl+0,i0

	add a0,null,c0	; ldx (i4),a1	//max gain
	//ldc 32768,a1
	ldc DECIM_DATA_RIGHT,I1
	ldx (i1),a0	; ldy (i6)+1,null	//data right
	ldc _agcConsts,i1
#ifdef VS1063
	call _AGCKeepDC
#else
	call _AGC
#endif
	ldc _adcControl+5,i0

	ldc _adcControl+10,i4
	add a0,null,d0	; ldx (i4),b0	//10 adcMode

/* Common branch again. */
$10:	lsr b0,b0			// check output mode!
#ifdef VS1063
	.import _audioPtr
	ldc _audioPtr+AUDIOPTR_WR,I7
#else
	.import _audio_wr_pointer
	ldc _audio_wr_pointer,I7
#endif
	jzc $23	//0,1 = Stereo
	nop
	jcs $11
	ldc _adcControl+0,i0
	//0 = Joint stereo
	//use the minimum positive of channel gains for joint stereo..
	ldx (i0)+5,a0
	ldx (i0),a1
	sub a0,a1,b0
	nop
	jgt $12
	nop
	mv a1,a0
$12:	stx a0,(i0)-5
	j $11
	stx a0,(i0)

$23:	jcs $2	//2=Left, 3=Right
	nop
	mv c0,d0	//2=Left  (r=l)
$2:	mv d0,c0	//3=Right (l=r)

$11:
	// disable interrupts again while restoring regs..
	ldc INT_GLOB_DIS,i5
	stx i5,(i5)	; ldy (i6)-1,I4
	mv c0,i5
	ldx (I6)-1,C0	; ldy (i6),B0
	ldx (I6)-1,A0	; ldy (I6),A1
	resp a0,a1
	ldx (I6)-1,A0	; ldy (I6),A1
	ldx (I6)-1,A2	; ldy (I6),B2
	ldx (I6)-1,C2	; ldy (I6),D2
	//directly to decimation
	.import _stream_wr_pointer
	ldc _stream_wr_pointer,i7
	ldc 0x8000+1024/*STREAM_BUFFER_SZ*/-1,i0
	ldy (i7),i1
	stx i5,(i1)*
	stx d0,(i1)*
	sty i1,(i7)
	ldx (i6)-1,i5	; ldy (i6),i0
	ldx (I6)-1,D0	; ldy (I6),I1
	// Surely the interrupt are disabled again by this instruction..
	ldx (i6)-1,lr1	; ldy (i6),lr0
	ldx (I6),MR0
	ldc INT_GLOB_ENA,i7
	reti
	stx i7,(i7)	; ldy (i6)-1,i7

	.end
