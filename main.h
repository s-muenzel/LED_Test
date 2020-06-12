#ifndef _LICHTWECKER_H
#define _LICHTWECKER_H

#define DEBUG_SERIAL

//#define IST_SONOFF
//#define IST_ESP01
//#define IST_NODEMCU32 // gilt auch für Lolin D32
#define IST_ESP32S


#ifdef IST_SONOFF
# define LED_AN LOW
# define LED_AUS HIGH
#else
# ifdef IST_ESP01
#  define LED_AN LOW
#  define LED_AUS HIGH
# else
#  if defined(IST_NODEMCU32) || defined(IST_ESP32)
#   define LED_AN LOW
#   define LED_AUS HIGH
#  else
#   define LED_AN HIGH
#   define LED_AUS LOW
#  endif
# endif
#endif

//
//#ifdef IST_ESP32S
//# define LED_PIN   13
//# define BLOCK_INTERUPTS portDISABLE_INTERRUPTS()
//# define UNBLOCK_INTERUPTS portENABLE_INTERRUPTS()
//# define USE_ESP32ENCODER
//// Pins für Rotary Encoder
//# define ROTARY_A_PIN  4 // braun
//# define ROTARY_B_PIN  2 // 35 // blau
//# define KNOPF_PIN     15 // schwarz
//#endif


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

const char HostName[] = "AussenLED";

#endif // _LICHTWECKER_H
