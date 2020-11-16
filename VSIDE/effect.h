#ifndef __EFFECT_H__
#define __EFFECT_H__
#include <vs1053.h>

#define EchoEffect
//#define FlangerEffect
//#define ReverbEffect
//#define ChorusEffect
//#define PureEchoEffect
//#define PhaserEffect

/*
  max delay = (8192-BLOCKSIZE)/24000 = 341ms (for mono signal, stereo has half delay)

  flanger = 5ms .. 20ms (= ~100 .. ~500)
  chorus = 15ms .. 100ms ( = ~500 .. ~2500
  delay = 100ms .. 341ms ( = ~2500 .. 8195)

  depth = depth of modulation (effect to delay length)
  delay = pre-delay (depth adds to this delay)
  speed = speed of modulation
    - add speed to phase every 64 (BLOCKSIZE) samples (24000Hz samplerate) -> 375 per/sec 
    - minimum frequency is 1/(2^16/375) = 0.00572204Hz
    - step is 0.00572204Hz
    - range 0 .. 1024 (5.7Hz)
  decay = amount of feedback from output to input ("repeat" control for delay effect)
  mix   = mix of dry and effect signal (0 == all dry, 32000 == all effect)

  flanger:
   depth = 1 .. 300
   speed = 1 .. 50
   delay = 0
   decay = 0 .. 32000
   mix = 0 .. 32000

  delay:
   depth = 0
   speed =0
   delay = 300 .. 8192
   decay = 0 .. 32000
   mix = 0 .. 32000

  "chorus":
   depth = 100 .. 1024
   speed 1 .. 50
   delay = 200 .. ~1000
   decay = 0 .. 32000
   mix = 0 .. 32000
*/

#ifdef EchoEffect
       #define BLOCKSIZE 64// 92 sometimes feedback //128 no sound and must reset//92 // 64 amount of samples processed in one time
       #define MY_SAMPLERATE 24000//48000//24000
       #define DELAY_BUF_SZ 8192//4096 Tick Tick //16384 No Mem//8192
       #define DEPTH 0//2000//500//0//800   // [0...2047]
       #define SPEED 0//40//30//0// 20   // LFO oscillator   
       #define DELAY 8000//100//500//2000 200 [0...8100] Delay of effect channel in samples.
                         // For stereo signal, the delay is only half of this 
       #define DECAY 28000//20000//20000//20000 //18000 // [0...32000] Speed of attenuation of the effect
       #define MIX 16000//2600 neverstops //16000//16000//16000 //16000   // [0...32000] Amount of delay/effect channel on output
#endif // EchoEffect

#ifdef PureEchoEffect
       #define BLOCKSIZE 64// 92 sometimes feedback //128 no sound and must reset//92 // 64 amount of samples processed in one time
       #define MY_SAMPLERATE 24000//48000//24000
       #define DELAY_BUF_SZ 8192//4096 Tick Tick //16384 No Mem//8192
       #define DEPTH 0//2000//500//0//800   // [0...2047]
       #define SPEED 0//40//30//0// 20   // LFO oscillator   
       #define DELAY 0//100//500//2000 200 [0...8100] Delay of effect channel in samples.
                         // For stereo signal, the delay is only half of this 
       #define DECAY 28000//20000//20000//20000 //18000 // [0...32000] Speed of attenuation of the effect
       #define MIX 16000//2600 neverstops //16000//16000//16000 //16000   // [0...32000] Amount of delay/effect channel on output
#endif // PureEchoEffect

#ifdef ChorusEffect
       #define BLOCKSIZE 64 // amount of samples processed in one time
       #define MY_SAMPLERATE 24000
       #define DELAY_BUF_SZ 8192
       #define DEPTH 800   // [0...2047]
       #define SPEED 50    // LFO oscillator   
       #define DELAY 400  // [0...8100] Delay of effect channel in samples.
                           // For stereo signal, the delay is only half of this 
       #define DECAY 20000 // [0...32000] Speed of attenuation of the effect
       #define MIX 16000   // [0...32000] Amount of delay/effect channel on output
#endif // ChorusEffect

#ifdef ReverbEffect
       #define BLOCKSIZE 64 // amount of samples processed in one time
       #define MY_SAMPLERATE 24000
       #define DELAY_BUF_SZ 8192
       #define DEPTH 0     // [0...2047]
       #define SPEED 0     // LFO oscillator   
       #define DELAY 2000  // [0...8100] Delay of effect channel in samples.
                           // For stereo signal, the delay is only half of this 
       #define DECAY 20000 // [0...32000] Speed of attenuation of the effect
       #define MIX 16000   // [0...32000] Amount of delay/effect channel on output
#endif // ReverbEffect

#ifdef FlangerEffect
       #define BLOCKSIZE 64  // amount of samples processed in one time
       #define MY_SAMPLERATE 24000 
       #define DELAY_BUF_SZ 8192 
       #define DEPTH 500    // [0...2047]
       #define SPEED 30     // LFO oscillator   
       #define DELAY 500    // [0...8100] Delay of effect channel in samples.
                            // For stereo signal, the delay is only half of this 
       #define DECAY 20000  // [0...32000] Speed of attenuation of the effect
       #define MIX 16000    // [0...32000] Amount of delay/effect channel on output
#endif // FlangerEffect

#ifdef PhaserEffect
       #define BLOCKSIZE 64  // amount of samples processed in one time
       #define MY_SAMPLERATE 24000 
       #define DELAY_BUF_SZ 8192 
       #define DEPTH 500    // [0...2047]
       #define SPEED 30     // LFO oscillator   
       #define DELAY 40    // [0...8100] Delay of effect channel in samples.
                            // For stereo signal, the delay is only half of this 
       #define DECAY 20000  // [0...32000] Speed of attenuation of the effect
       #define MIX 16000    // [0...32000] Amount of delay/effect channel on output
#endif // PhaserEffect

#endif // __EFFECT_H__
