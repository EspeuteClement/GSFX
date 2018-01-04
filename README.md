# GSFX : Gamebuino Sound FX library

Hello, this is GSFX, a small and simple library that allows you to play simple sound fx in your games with a tiny cpu footprint (around 5% of CPU).

## How To Use

1. Copy `GSFX.h` and `GSFX.cpp` into your arduino sketch folder.
2. Include `GSFX.h` where you want : `#include "GSFX.h"`
3. Create a `GSFX` object : `GSFX gsfx;`
4. After the `gb.begin();`, put `gsfx.init();`
5. Now play sound with `gsfx.play({GSFX::WaveType::SQUARE, 0x8000, 0, 8192,0,5000});`

## GSFX::FX

This structure has 6 fields :
* `GSFX::WaveType type` Either GSFX::WaveType::SQUARE (melodic tones) or GSFX::WaveType::NOISE (gritty noises).
* `int32_t volume_start` At with amplitude the volume of the sound starts. This value is encoded as an 8bit fixed point number, meaning that you have to right shift by 8 bit yours values.
* `int32_t volume_sweep` How much the volume change PER 4 SAMPLE. A value of 1 means that for each sample, the volume will increase by one.
* `int32_t period_start` The period of the waveform at wich the fx stars. A value of 0 means the highest pitch possible.
* `int32_t period_sweep` How much the period changes per 4 sample.
*  `int32_t length` The lenght in samples of the FX.

## Playing FX chains
You can chain FX together for more complex sounds (usefull for playing two different tones for example).

`void GSFX::Sound_Handler_GSFX::play(const GSFX::FX * const pattern, uint8_t length)`

With pattern an array of FX and length the number of FX in the pattern.

Example :
```c++
const GSFX::FX chain[] = 
{
    {GSFX::WaveType::SQUARE, 32768, 0,8192, 0,3000},
    {GSFX::WaveType::SQUARE, 32768, -16,4096, 0,6000}
};

gsfx.play(chain, 2);
```


## GSFX Tool Manual
You edit a Pattern. Each Pattern are composed of maximum 8 FX. An FX is a Simple sound.

* <kbd>A</kbd> Plays the current Pattern
* <kbd>UP</kbd>/<kbd>DOWN</kbd> Changes the current parameter
* <kbd>LEFT</kbd>/<kbd>RIGHT</kbd> changes the current parameter value. Hold B to change faster. On Pediod, holding <kbd>B</kbd> allows you to multiply or divide by two your perdiod (changes the octave).
* Pattern POS Changes the current FX you edit
* PATTERN NUM Changes the number of FX in the current pattern
* SAVE SLOT changes the current PATTERN you edit in the save file (up to 8)
* COPY/PASTE : <kbd>LEFT</kbd>/<kbd>RIGHT</kbd> to change between COPY and PASTE. Press <kbd>B</kbd> to perform the selected action. COPY copies the current FX, PASTE pastes the previouly copied FX.
* SAVING : Press MENU to save.

