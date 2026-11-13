#pragma config OSC = HSPLL
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config BOREN = OFF
#pragma config WDT = OFF
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config XINST = OFF
#pragma config DEBUG = OFF

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

#include <xc.h>
#include <stdint.h>
#include "map.h"
#include "game.h"
#include "scoring.h"

#define TMR0H_VAL 0xF8
#define TMR0L_VAL 0x5F

volatile uint8_t flag_flap = 0;
volatile uint8_t flag_speed_up = 0;
volatile uint8_t flag_speed_down = 0;
volatile uint8_t tick_50ms = 0;
volatile uint32_t timer_ms = 0;
volatile uint16_t scroll_step_time = 1000;

void InitMCU(void) {
    INTCONbits.GIE = 0;
    RCONbits.IPEN = 0;
    TRISB |= 0x0B;

    TRISC = 0x00; LATC = 0x00;
    TRISD = 0x00; LATD = 0x00;
    TRISE = 0x00; LATE = 0x00;
    TRISF = 0x00; LATF = 0x00;

    TRISH = 0x00; LATH = 0x00;
    TRISJ = 0x00; LATJ = 0x00;

    INTCONbits.INT0IE  = 1;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT3IE = 1;

    INTCON2bits.INTEDG0 = 1;
    INTCON2bits.INTEDG1 = 1;
    INTCON2bits.INTEDG2 = 1;
    INTCON2bits.INTEDG3 = 1;

    INTCONbits.INT0IF  = 0;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT3IF = 0;

    T0CON = 0b00000111;
    TMR0H = TMR0H_VAL;
    TMR0L = TMR0L_VAL;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON  = 0;
}

__interrupt(high_priority)
void HandleInterrupt(void)
{
    if (INTCONbits.INT0IE && INTCONbits.INT0IF) {
        flag_flap = 1;
        INTCONbits.INT0IF = 0;
    }

    if (INTCON3bits.INT1IE && INTCON3bits.INT1IF) {
        flag_speed_down = 1;
        INTCON3bits.INT1IF = 0;
    }

    if (INTCON3bits.INT3IE && INTCON3bits.INT3IF) {
        flag_speed_up = 1;
        INTCON3bits.INT3IF = 0;
    }

    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        TMR0H = TMR0H_VAL;
        TMR0L = TMR0L_VAL;
        tick_50ms = 1;
        timer_ms += 50;
        INTCONbits.TMR0IF = 0;
    }
}

void main(void)
{
    uint32_t last_scroll_ms = 0;
    uint32_t last_gravity_ms = 0;
    uint32_t previous_ms = 0;

    InitMCU();
    reset_score();
    for (uint16_t ms = 0; ms < 1000; ms++) {
        display_score();
        __delay_ms(1);
    }

    game_init();
    TMR0H = TMR0H_VAL;
    TMR0L = TMR0L_VAL;
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    while (1) {
        uint32_t now;
        uint32_t elapsed;
        uint8_t flap;
        uint8_t speed_up;
        uint8_t speed_down;
        uint8_t blink;
        uint8_t old_gie = INTCONbits.GIE;

        INTCONbits.GIE = 0;
        now = timer_ms;
        flap = flag_flap;
        speed_up = flag_speed_up;
        speed_down = flag_speed_down;
        blink = tick_50ms;
        flag_flap = 0;
        flag_speed_up = 0;
        flag_speed_down = 0;
        tick_50ms = 0;
        INTCONbits.GIE = old_gie;

        elapsed = now - previous_ms;
        previous_ms = now;

        if (!game_is_running()) {
            tick_ms(elapsed);
            if (game_is_running()) {
                last_scroll_ms = now;
                last_gravity_ms = now;
            }
            display_score();
            continue;
        }

        if (speed_up && scroll_step_time > 250) {
            scroll_step_time -= 250;
        }
        if (speed_down && scroll_step_time < 1500) {
            scroll_step_time += 250;
        }

        if (flap) {
            bird_flap();
            if (handle_collision()) {
                render();
                display_score();
                continue;
            }
            render();
        }

        if (blink) {
            bird_toggle();
            render();
        }

        if (now - last_scroll_ms >= scroll_step_time) {
            last_scroll_ms = now;
            update_score();
            scene_scroll();
            if (handle_collision()) {
                render();
                display_score();
                continue;
            }
            render();
        }

        if (now - last_gravity_ms >= 750) {
            last_gravity_ms += 750;
            bird_gravity();
            if (handle_collision()) {
                render();
                display_score();
                continue;
            }
            render();
        }

        update_alive_score(now);
        display_score();
    }
}
