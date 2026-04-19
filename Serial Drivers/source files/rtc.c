/*
 * rtc.c
 * Author: Seth Lee
 * Description: RTC driver
 */

#include "rtc.h"

// RCC / PWR enables
#define PWREN       (1U << 28)   // APB1ENR: power interface clock
#define CR_DBP      (1U << 8)    // PWR_CR: disable backup domain write protection
#define CSR_LSION   (1U << 0)    // RCC_CSR: LSI enable
#define CSR_LSIRDY  (1U << 1)    // RCC_CSR: LSI ready flag
#define BDCR_BDRST  (1U << 16)   // RCC_BDCR: backup domain reset
#define BDCR_RTCEN  (1U << 15)   // RCC_BDCR: RTC enable

// RTC write protection keys
#define WPR_KEY1    0xCA
#define WPR_KEY2    0x53

// RTC ISR flags
#define ISR_INITF   (1U << 6)    // init mode flag
#define ISR_INIT    (1U << 7)    // enter init mode
#define ISR_RSF     (1U << 5)    // registers sync flag

// Prescalers for LSI (~32kHz)
// 32000 / (127+1) / (249+1) = 1Hz
#define ASYNCH_PREDIV   127
#define SYNCH_PREDIV    249

// Convert decimal to BCD — e.g. 59 -> 0x59
static uint8_t dec_to_bcd(uint8_t val) {
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}

// Convert BCD to decimal — e.g. 0x59 -> 59
static uint8_t bcd_to_dec(uint8_t val) {
    return (uint8_t)(((val >> 4) * 10) + (val & 0x0F));
}

static void rtc_enter_init(void) {
    RTC->ISR |= ISR_INIT;
    while (!(RTC->ISR & ISR_INITF)) {}  // wait until in init mode
}

static void rtc_exit_init(void) {
    RTC->ISR &= ~ISR_INIT;
    // wait for shadow registers to sync
    while (!(RTC->ISR & ISR_RSF)) {}
}

void rtc_init(void) {
    // 1. Enable PWR clock
    RCC->APB1ENR |= PWREN;

    // 2. Enable backup domain write access
    PWR->CR |= CR_DBP;

    // 3. Enable LSI oscillator
    RCC->CSR |= CSR_LSION;
    while (!(RCC->CSR & CSR_LSIRDY)) {}  // wait for LSI to stabilize

    // 4. Reset backup domain to clear any previous RTC config
    RCC->BDCR |= BDCR_BDRST;
    RCC->BDCR &= ~BDCR_BDRST;

    // 5. Select LSI as RTC clock source (bits [9:8] = 10)
    RCC->BDCR &= ~(1U << 8);
    RCC->BDCR |=  (1U << 9);

    // 6. Enable RTC
    RCC->BDCR |= BDCR_RTCEN;

    // 7. Unlock RTC write protection
    RTC->WPR = WPR_KEY1;
    RTC->WPR = WPR_KEY2;

    // 8. Enter init mode
    rtc_enter_init();

    // 9. Set prescalers (must be in init mode)
    RTC->PRER = (ASYNCH_PREDIV << 16) | SYNCH_PREDIV;

    // 10. Set 24hr format (clear FMT bit)
    RTC->CR &= ~(1U << 6);

    // 11. Exit init mode
    rtc_exit_init();

    // 12. Re-enable write protection
    RTC->WPR = 0xFF;
}

void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // Unlock
    RTC->WPR = WPR_KEY1;
    RTC->WPR = WPR_KEY2;

    rtc_enter_init();

    // Time register: [21:16]=hours BCD, [14:8]=minutes BCD, [6:0]=seconds BCD
    RTC->TR = ((uint32_t)dec_to_bcd(hours)   << 16) |
              ((uint32_t)dec_to_bcd(minutes)  << 8)  |
              ((uint32_t)dec_to_bcd(seconds));

    rtc_exit_init();

    RTC->WPR = 0xFF;
}

void rtc_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    // Reading RTC_TR latches RTC_DR — must read TR first
    uint32_t tr = RTC->TR;

    *hours   = bcd_to_dec((tr >> 16) & 0x3F);
    *minutes = bcd_to_dec((tr >> 8)  & 0x7F);
    *seconds = bcd_to_dec( tr        & 0x7F);
}
