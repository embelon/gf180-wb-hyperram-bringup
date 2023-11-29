
#ifndef PLL_H
#define PLL_H

#include <stdint.h>

void pll_enable(uint8_t dco_mode);
void pll_disable();

// frequency = (OSC * mult) / div / div2
void pll_configure(uint8_t mult, uint8_t div, uint8_t div2);

void pll_change_clock(uint8_t pll_1_vs_osc_0);

#endif // PLL_H