
#include <defs.h>
#include <pll.h>

void pll_enable(uint8_t dco_mode)
{
    // free_running = DCO
    reg_hkspi_pll_ena = (dco_mode ? 2 : 0) | 1;
}

void pll_disable()
{
    // enable bypass to keep device clocked
    reg_hkspi_pll_bypass = 1;

    // now disable PLL
    reg_hkspi_pll_ena = 0;
}

// frequency = (OSC * mult) / div / div2
void pll_configure(uint8_t mult, uint8_t div, uint8_t div2)
{
    if (reg_hkspi_pll_bypass & 1 == 0)
    {
        // do not change frequency while device is already clocked from PLL
        return;
    }

    if (reg_hkspi_pll_ena & 1 == 0)
    {
        pll_enable(0);
    }

    mult &= 0x1f;   // only 5bits
    div &= 0x07;    // only 3bits
    div2 &= 0x07;   // obnly 3bits
    
    reg_hkspi_pll_source = mult;
    reg_hkspi_pll_divider = (div2 << 3) | div;
}

void pll_change_clock(uint8_t pll_1_vs_osc_0)
{
    if (pll_1_vs_osc_0 && (reg_hkspi_pll_ena & 1 == 0))
    {
        // can't switch to PLL output, as PLL is not enabled
        return;
    }

    reg_hkspi_pll_bypass = pll_1_vs_osc_0 ? 0 : 1;
}