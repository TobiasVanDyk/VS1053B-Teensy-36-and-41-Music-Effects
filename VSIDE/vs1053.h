#ifndef __VS1053_H_
#define __VS1053_H_

#ifndef VS1053
#define VS1053
#endif

#define DAC_DEFAULT_SAMPLERATE 8000
#define DAC_DRIVER_ON_DELAY (DAC_DEFAULT_SAMPLERATE/10) /* In samples */

#define MR_NONE 0
#define MR_INT 0x200
#define MR_SAT 0x400

#define AUDIO_BUFFER_SZ  4096
#define STREAM_BUFFER_SZ 1024

#define AUDIOB_LOC_Y  0x0000
#define STREAM_LOC_X  0x0000
#define YPREV0_LOC_X  0x0400
#define OTHERS_LOC_X  (YPREV0_LOC_X+0x400)  //0x0800 must be after YPREV0
#define SAMPLE_LOC_XY (OTHERS_LOC_X+0x800)  //0x1000 should be after OTHERS

#ifdef VS1053 /* */

#define IROM_START 0x2000
#define IROM_SIZE  0xe000
#define IROM_START2 0xc000 /*plus overlapping block at 0xc000 for MIDI */
#define IROM_SIZE2  0x4000
#define XROM_START 0x4000
#define XROM_SIZE  0x8000
#define XROM_START2 0xc100
#define XROM_SIZE2  0x3f00 /*plus another block at 0xc100*/
#define YROM_START 0x4000
#define YROM_SIZE  0xa000

#define XRAM_START    0x0
#define XRAM_SIZE  0x4000
#define YRAM_START    0x0
#define YRAM_SIZE  0x4000
#define YRAM_START2 0xe000
//#define YRAM_SIZE2  0x1000 /* Was wrong in VS1053B */
#define YRAM_SIZE2  0x2000 /* this is not cleared at startup */
#define IRAM_START    0x0
#define IRAM_SIZE  0x1000

#define YPREV1_LOC_X  0x1a00

#else /*VS1053*/

#define IROM_START 0x4000
#define XROM_START 0x4000
#define YROM_START 0x4000
#define XRAM_START    0x0
#define YRAM_START    0x0
#define IRAM_START    0x0

#ifdef VS1003

#define IROM_SIZE  0x8000
#define XROM_SIZE  0x8000
#define YROM_SIZE  0x4000
#define XRAM_SIZE  0x1e00
#define YRAM_SIZE  0x1c00
#define IRAM_SIZE   0x500

#define YPREV1_LOC_X  0x1a00

#else /*!VS1003, VS1023*/

#define IROM_SIZE  0xc000
#define XROM_SIZE  0x8000
#define YROM_SIZE  0x8000
#define XRAM_SIZE  0x2000
#define YRAM_SIZE  0x2800
#define IRAM_SIZE   0x500

#define YPREV1_LOC_X  0x1c00

#endif/*VS1023*/

#endif/*VS1053else*/

#ifdef VS1053
#define USER_AREA_START 0x1800
#define USER_AREA_SIZE 0x80

#define STACK_START    0x1880
#define STACK_SIZE     (0xc0+0x40)

#else

#define USER_AREA_START 0x1800
#define USER_AREA_SIZE 0x80
#define STACK_START    0x1880
#define STACK_SIZE     0xc0
#endif
#define DEBUG_STACK    0x1920  /* tarvitaan ainakin 18 + temppitilaa */


#define BEHAVE_LIMIT 8
#define ONE_FREQ_LIMIT 16383 /* May NOT be more than 16383: asmfuncs.s/Magic3*/
#define SUMLIMIT 40000

#define INT_EN_SDM  9
#define INT_EN_SRC  8
#define INT_EN_TIM1 7
#define INT_EN_TIM0 6
#define INT_EN_RX   5
#define INT_EN_TX   4
#define INT_EN_MODU 3
#define INT_EN_SDI  2
#define INT_EN_SCI  1
#define INT_EN_DAC  0

#define INT_EN_NONE 0
#define INT_EN_ALL ((1<<INT_EN_DAC)|(1<<INT_EN_SCI)|(1<<INT_EN_SDI)|(1<<INT_EN_TX)|(1<<INT_EN_RX))

#ifndef ASM
#ifndef USEY
#define USEY(x) (*(volatile u_int16 __y *)(u_int16)(x))
#endif
#ifndef USEX
#define USEX(x) (*(volatile u_int16 __x *)(u_int16)(x))
#endif
#endif

#define SCI_BASE	0xC000
#define SCI_MODE	0xC000
#define SCI_STATUS	0xC001
#define SCI_BASS	0xC002
#define SCI_CLOCKF	0xC003
#define SCI_DECODE_TIME	0xC004
#define SCI_AUDATA	0xC005
#define SCI_WRAM	0xC006
#define SCI_WRAMADDR	0xC007
#define SCI_HDAT0	0xC008
#define SCI_HDAT1	0xC009
#define SCI_AIADDR	0xC00A
#define SCI_VOL		0xC00B
#define SCI_AICTRL0	0xC00C
#define SCI_AICTRL1	0xC00D
#define SCI_AICTRL2	0xC00E
#define SCI_AICTRL3	0xC00F
#define SCI_CHANGE	0xC010
#define SER_DATA	0xC011	/* R */
#define SER_DREQ	0xC012	/* RW, LSB controls DREQ pin */
#define FREQCTLL	0xC013
#define FREQCTLH	0xC014
#define DAC_LEFT	0xC015
#define DAC_RIGHT	0xC016
#define GPIO_DDR	0xC017
#define GPIO_IDATA	0xC018
#define GPIO_ODATA	0xC019
#define INT_ENABLE	0xC01A	/* RW */
#define INT_GLOB_DIS	0xC01B	/* W  */
#define INT_GLOB_ENA	0xC01C	/* W  */
#define INT_COUNTER	0xC01D	/* R  */
#define MODU_DIV	0xC01E
#define MODU_DATA	0xC01F

#define WDOG_CONFIG	0xC020
#define WDOG_RESET	0xC021
#define WDOG_DUMMY	0xC022
#define WDOG_RESET_VAL	0x4ea9

#define UART_STATUS	0xC028
#define UART_DATA	0xC029
#define UART_DATAH	0xC02A
#define UART_DIV	0xC02B

#define TIMER_CONFIG	0xC030
#define TIMER_ENABLE	0xC031
#define TIMER_T0L	0xC034
#define TIMER_T0H	0xC035
#define TIMER_T0CNTL	0xC036
#define TIMER_T0CNTH	0xC037
#define TIMER_T1L	0xC038
#define TIMER_T1H	0xC039
#define TIMER_T1CNTL	0xC03A
#define TIMER_T1CNTH	0xC03B

#define TIMER_EN_T0_B 0
#define TIMER_EN_T1_B 1

#define TIMER_EN_T0 (1<<TIMER_EN_T0_B)
#define TIMER_EN_T1 (1<<TIMER_EN_T1_B)

#define I2S_CONTROL     0xC040
#define I2S_MCLK_ENA 8
#define I2S_TX_ENA   4
#define I2S_48K      0
#define I2S_96K      1
#define I2S_192K     2

#define FUSE_REG        0xC041
#define FUSE_BIT        (1<<5)


/*
  -- -------------------------------
  --  New peripherals to VS1053  --
  -- -------------------------------
  -- C042         - decim_div : decimation factor and decimator enable
  -- C043         - decim_data left : decimator data value, READ ONLY
  -- C044         - decim_data right : decimator data value, READ ONLY
  -- C045         - dac_vol_rs
  -- C046         - src control   (src_enable_rs(12) & src_limit_rs(11:0))
  -- C047         - src left data
  -- C048         - src right data
  -- C049         - sdm control   (sdm_enable_rs(12) & sdm_limit_rs(11:0))
  -- C04A         - sdm left data   (sdm_l_offset_rs)
  -- C04B         - sdm right data  (sdm_r_offset_rs)
  -- C04C         - mac ram test mode, irom4 select (both are active high)

  -- NOTE: MACRAM TEST CAN ONLY BE EXECUTED ON 12MHZ CLOCK MODE 
  --       (i.e. 1x dsp clock)
  -- MACRAM TEST enable
  --    - dspif 0xC04C, bit 1         = mram_testmode_rs (active high)
  -- MACRAM TEST CTRL ADDRESS
  --    - dspif 0xC04A, bits (13:0)   = sdm_l_offset_rs
  --        [15]   : rd  => decim_factor(1)
  --        [14]   : wr  => decim_factor(0)
  --        [13:8] : wr_data6   => 6 x 6 bits to mac ram (35:30) & (29:24) ...
  --        [7:6]  : data column address for READ
  --                 "00"/"11" = [35:32], "01" = [15:0], "10" = [31:16]
  --        [5:0]  : macram address
  -- MACRAM TEST DATA READ ADDRESS
  --    - dspif 0xC043                = decim_ldata
  -- 
*/
#define DECIM_CONTROL    0xc042
#define DECIM_MODU2_PD       16
#define DECIM_MODU1_PD        8
#define DECIM_FACTOR24K       6 // 24 kHz f256
#define DECIM_FACTOR48K       4 // 48 kHz f128
#define DECIM_FACTOR96K       2 // 96 kHz f64
#define DECIM_FACTOR192K      0 //192 kHz f32
#define DECIM_ENABLE          1

#define DECIM_DATA_LEFT  0xc043
#define DECIM_DATA_RIGHT 0xc044
#define DAC_VOL          0xc045
#define SRC_CONTROL      0xc046
#define SRC_ENABLE       (1<<12)
#define SRC_DATA_LEFT    0xc047
#define SRC_DATA_RIGHT   0xc048
#define SDM_CONTROL      0xc049
#define SDM_ENABLE       (1<<12)
#define SDM_DATA_LEFT    0xc04A
#define SDM_DATA_RIGHT   0xc04B
#define VS1053_IROM4 0xc04c /*mac ram test mode bit 1, irom4 select bit 0 */
#define MRAM_TESTMODE         2
#define IROM4_ENABLE          1
/*

  Pin mappings:
  GPIO0 - old TEST1 - e.g. _CS
  GPIO1 - old TEST2 -      MOSI
  GPIO2 - DCLK      -      CLK
  GPIO3 - SDATA     -      MISO

*/
#define SCIMB_DIFFERENTIAL	 0
#define SCIMB_ALLOW_LAYER12	 1
#define SCIMB_SOFT_RESET	 2
#define SCIMB_OUT_OF_WAV	 3
#ifdef USE_EARSPEAKER
#define SCIMB_EARSPEAKER_LO      4
#else
#define SCIMB_POWERDOWN		 4
#endif
#define SCIMB_ALLOW_TESTS	 5
#define SCIMB_STREAM_MODE	 6
#ifdef USE_EARSPEAKER
#define SCIMB_EARSPEAKER_HI	 7
#define SCIMF_EARSPEAKER_HILO ((1<<SCIMB_EARSPEAKER_LO)|(1<<SCIMB_EARSPEAKER_HI))
#else
#define SCIMB_PLUSV		 7
#define SCIMB_ALLOW_DRM SCIMB_PLUSV
#endif
#define SCIMB_DCLK_ACTIVE_EDGE	 8
#define SCIMB_SDI_BYTE_ORDER	 9
#define SCIMB_SDI_SHARED	10
#define SCIMB_SDI_NEWMODE	11
#define SCIMB_ADPCM		12
#define SCIMB_ADPCM_HP		13
#define SCIMB_LINE		14
#define SCIMB_CLK_IN_DIV	15

#ifdef VS1053
#define SCIST_REFERENCE_SEL 0
#define SCIST_AD_CLOCK 1
#define SCIST_VCM_OVERLOAD 11
#define SCIST_VCM_DISABLE 10
#define SCIST_SWING 12 //14..12
#else
#define SCIST_MINUS06	0
#define SCIST_MINUS12	1
#endif
#define SCIST_DO_NOT_JUMP 15
#define SCIST_APDOWN1	2	/* Analog internal */
#define SCIST_APDOWN2	3	/* Driver */
#define SCIST_VER0	4	/* Version number 0..7 */
#define SCIST_VER1	5	/* Upto VS1001k = 0, VS1011 = 1, VS1002 = 2 */
#define SCIST_VER2	6
#define SCIST_VER3      7

#define SCIW_OFFSET (SCI_BASE-0x10)

#define UART_ST_RXORUN    (1<<3)
#define UART_ST_RXFULL    (1<<2)
#define UART_ST_TXFULL    (1<<1)
#define UART_ST_TXRUNNING (1<<0)

#define FCH_MUL0_B 4
#define FCH_MUL1_B 5
#define FCH_MUL2_B 6
#define FCH_MUL3_B 7
#define FCH_DIV_INCLK_B 8
#define FCH_FORCE_PLL_B 9
//#define FCH_AUTOMODE_B 10
#define FCH_VCO_OUT_ENA_B 11
#define FCH_PLL_SET_LOCK_B 12
#define FCH_PLL_LOCKED_B 13

#define FCH_MUL0 (1<<FCH_MUL0_B)
#define FCH_MUL1 (1<<FCH_MUL1_B)
#define FCH_MUL2 (1<<FCH_MUL2_B)
#define FCH_MUL3 (1<<FCH_MUL3_B)
#define FCH_DIV_INCLK (1<<FCH_DIV_INCLK_B)
#define FCH_FORCE_PLL (1<<FCH_FORCE_PLL_B)
//#define FCH_AUTOMODE (1<<FCH_AUTOMODE_B)
#define FCH_VCO_OUT_ENA (1<<FCH_VCO_OUT_ENA_B)
#define FCH_PLL_SET_LOCK (1<<FCH_PLL_SET_LOCK_B)
#define FCH_PLL_LOCKED (1<<FCH_PLL_LOCKED_B)


#define USER_CODEC_ADDR 0
#define USER_CODEC ((void (*) (void))USER_CODEC_ADDR)

#ifndef ASM


#include "vsmpg.h"

#ifndef VS1003

#define FEATURE_NO_WMA  31
#define FEATURE_NO_AAC  30
#define FEATURE_NO_MIDI 29
#define FEATURE_NO_MP3  28

u_int16 ReadFuse(register __a0 num); /* does not perform interrupt protect */
#endif /*!VS1003*/

/*void DacInt(void);*/
auto u_int16 SciInt(register __a0 u_int16 dreqBeforeSci);
/*void DataInt(void);*/
auto void InitHardware(void);
auto void SetHardware(register s_int16 channels, register u_int16 rate);
void SetRate(register __c1 u_int16 rate);

extern __y u_int16 bassReg, volReg;
auto void CheckVolumeAndBass(void);
auto void SetVolumeAndBass(void);

auto void SetClockFromSpi(void);
u_int16 UartDiv(void);

extern s_int16 __y audio_buffer[];

volatile extern s_int16 __y * audio_wr_pointer;
volatile extern s_int16 __y * audio_rd_pointer;
volatile extern s_int16 __y * audio_underflow;
extern u_int16 stream_buffer[];
volatile extern u_int16 __x * __y stream_wr_pointer;
volatile extern u_int16 __x * __y stream_rd_pointer;
volatile extern s_int16 __y stream_rd_odd;
extern const s_int16 halfDB[12];

extern __y u_int16 hwSampleRate;
extern u_int32 __y curFctl;
extern u_int16 __y bassOn, trebleOn, uartByteSpeed;
extern s_int16 __y volume[2];

extern s_int16 __y streamMode;
extern s_int16 __y streamModeLastRate;
extern const u_int16 streamModeLimits[16];

extern u_int16 __y dreqAfterSci;

auto u_int16 DataBytes(void);

auto s_int16 AudioBufFill(void);
auto s_int16 StreamDiff(void);

void CheckHwPointers(void);

extern u_int16 __y extClock4KHz,intClock2KHz,clockX;

#ifdef VS1053
void CallIROM4(register __a0 void *address);
#endif/*VS1053*/

void Reset(void);

// added for VS1053 examples (Las)

auto s_int16 AudioOutputSamples(s_int16 *buf,s_int16 cnt);
auto void StreamBufferReadData(register __i0 __y void *p, register __a0 s_int16 n);

extern s_int16 (*applAddr)(s_int16 register __i0 **d,s_int16 register __a1 mode,s_int16 register __a0 n);

void Disable(void);
void Enable(void);

struct AGC {
	s_int16 gain;
	u_int32 lpRes;
	s_int32 offset;
};

extern struct AdcControl {
	struct AGC agc[2];
	s_int16 adcMode;
	s_int16 encMode;
} adcControl;

extern __y s_int16 agcConsts[7]; /* copied to YRAM at IMA startup to allow changes */
extern const s_int16 agcConstsInit[7];

#endif // ASM

#define CORE_CLOCK_1X		0x0000
#define CORE_CLOCK_2X		0x2000
#define CORE_CLOCK_2_5X	0x4000
#define CORE_CLOCK_3X		0x6000
#define CORE_CLOCK_3_5X	0x8000
#define CORE_CLOCK_4X		0xA000
#define CORE_CLOCK_4_5X	0xC000
#define CORE_CLOCK_5X		0xE000

#endif // __VS1053_H_
