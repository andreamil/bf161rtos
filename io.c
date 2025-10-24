#include <pic18f4550.h>

#include "io.h"


void set_channel(channel_t channel)
{
    ADCON0bits.CHS = channel;
}

void set_port(port_conf_t port)
{
    ADCON1bits.PCFG = port;
}

void config_adc(tad_t tad, conversion_clock_t cclk)
{
    ADCON2bits.ACQT = tad;
    ADCON2bits.ADCS = cclk;
    ADCON2bits.ADFM = 1;
}

void adc_go(int go_done)
{
    ADCON0bits.ADON = go_done;
}

int adc_read()
{
    ADCON0bits.GO = 1;
    while (ADCON0bits.GODONE);
    return ADRES;
}

// ---------------- PWM (software) -----------------ad
static volatile uint8_t _pwm_period = 255;
static volatile uint8_t _pwm_phase = 0;
static volatile uint8_t _pwm_duty[4] = {0,0,0,0};

void pwm_init(uint8_t period)
{
    _pwm_period = (period == 0) ? 255 : period;
    _pwm_phase = 0;
    _pwm_duty[0] = _pwm_duty[1] = _pwm_duty[2] = _pwm_duty[3] = 0;

    // Configura RD1..RD4 como saída (motores)
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
    TRISDbits.RD4 = 0;

    LATDbits.LATD1 = 0;
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 0;
    LATDbits.LATD4 = 0;
}

void pwm_set_duty(pwm_channel_t ch, uint8_t duty)
{
    if (ch > PWM_CH4) return;
    if (duty > _pwm_period) duty = _pwm_period;
    _pwm_duty[(uint8_t)ch] = duty;
}

void pwm_tick(void)
{
    // Avança fase e atualiza saídas
    _pwm_phase++;
    if (_pwm_phase >= _pwm_period) _pwm_phase = 0;

    LATDbits.LATD1 = (_pwm_duty[0] > _pwm_phase) ? 1 : 0;
    LATDbits.LATD2 = (_pwm_duty[1] > _pwm_phase) ? 1 : 0;
    LATDbits.LATD3 = (_pwm_duty[2] > _pwm_phase) ? 1 : 0;
    LATDbits.LATD4 = (_pwm_duty[3] > _pwm_phase) ? 1 : 0;
}

// -------------- Interrupção Externa (INT0) --------------
static void (*_exti_cb)(void) = 0;

void exti_init(uint8_t rising_edge)
{
    // INT0 em RB0
    TRISBbits.RB0 = 1; // entrada
    INTCON2bits.INTEDG0 = rising_edge ? 1 : 0; // 1 borda de subida, 0 descida
    INTCONbits.INT0IF = 0; // limpa flag
}

void exti_enable(uint8_t enable)
{
    INTCONbits.INT0IE = enable ? 1 : 0;
}

void exti_set_callback(void (*cb)(void))
{
    _exti_cb = cb;
}

void exti_isr(void)
{
    if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        if (_exti_cb) _exti_cb();
    }
}

