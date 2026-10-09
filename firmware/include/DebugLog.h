/**
 * @file DebugLog.h
 * @brief Mirrors boot/status log lines to both UART0 (Serial1) and the
 *        native USB CDC port (Serial), so `test_lora.sh` — which reads
 *        /dev/ttyACM0 / /dev/ttyACM1 — can observe them without requiring
 *        a UART-to-USB adapter wired to GP0/GP1.
 */

#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <Arduino.h>

inline void bravoLog(const String &msg) {
    Serial1.println(msg);
    Serial.println(msg);
}

#endif // DEBUG_LOG_H
