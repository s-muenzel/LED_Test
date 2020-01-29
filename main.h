#ifndef _LICHTWECKER_H
#define _LICHTWECKER_H

//#define DEBUG_SERIAL

//#define IST_SONOFF
#define IST_ESP01


#ifdef IST_SONOFF
# define LED_AN LOW
# define LED_AUS HIGH
#else
# ifdef IST_ESP01
#  define LED_AN LOW
#  define LED_AUS HIGH
# else
#  define LED_AN HIGH
#  define LED_AUS LOW
# endif
#endif


#ifdef DEBUG_SERIAL
#ifndef D_BEGIN
#define D_BEGIN(speed)   Serial.begin(speed)
#define D_PRINT(...)     Serial.print(__VA_ARGS__)
#define D_PRINTLN(...)   Serial.println(__VA_ARGS__)
#define D_PRINTF(...)    Serial.printf(__VA_ARGS__)
#endif // ifndef D_BEGIN
#else
#define D_BEGIN(speed)
#define D_PRINT(...)
#define D_PRINTLN(...)
#define D_PRINTF(...)
#endif

#endif // _LICHTWECKER_H
