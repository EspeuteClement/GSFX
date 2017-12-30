# GSFX : Gamebuino Sound FX library

Hello, this is GSFX, a small and simple library that allows you to play simple sound fx in your games with a tiny cpu footprint (around 5% of CPU).

## How To Use

1. Copy `GSFX.h` and `GSFX.cpp` into your arduino sketch folder.
2. Include `GSFX.h` where you want : `#include "GSFX.h"`
3. Create a `GSFX` object : `GSFX gsfx;`
4. After the `gb.begin();`, put `gsfx.init();`
5. Now play sound with `gsfx.play({GSFX::WaveType::SQUARE, 0x8000, 0, 8192,0,5000});`

## GSFX::Pattern

This structure has 6 fields :
* `GSFX::WaveType type` Either GSFX::WaveType::SQUARE (melodic tones) or GSFX::WaveType::NOISE (gritty noises).
* `int32_t volume_start` At with amplitude the volume of the sound starts. This value is encoded as an 8bit fixed point number, meaning that you have to right shift by 8 bit yours values.
* `int32_t volume_sweep` How much the volume change PER 4 SAMPLE. A value of 1 means that for each sample, the volume will increase by one.
* `int32_t period_start` The period of the waveform at wich the fx stars. A value of 0 means the highest pitch possible.
* `int32_t period_sweep` How much the period changes per 4 sample.
*  `int32_t length` The lenght in samples of the FX.