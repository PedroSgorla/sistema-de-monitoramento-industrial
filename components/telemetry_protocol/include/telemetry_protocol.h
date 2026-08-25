/**
 * @file telemetry_protocol.h
 * @brief Definição do protocolo de telemetria para comunicação CAN.
 */
#ifndef TELEMETRY_PROTOCOL_H
#define TELEMETRY_PROTOCOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status do sistema(armazenamento de um byte no pacote ) ---- */
enum {
    STATUS_OK = 0,
    STATUS_ACTIVE = 1,
    STATUS_EMERGENCY = 2,
    STATUS_CAN_COMMUNICATION_FAILURE = 3,
};

/* ---- IDs CAN ---- */
enum {
    CAN_ID_EMERGENCY = 0x001,
    CAN_ID_HEARTBEAT = 0x002,
    CAN_ID_VELOCITY_COMMAND = 0x010,
    CAN_ID_SLOW_STOP = 0x011,
    CAN_ID_TELEMETRY = 0x012,
};

/* ---- Contrato bytes compartilhado pelos 3 nós ---- */
typedef struct __attribute__((packed)) {
    uint32_t package_id;       /* offset 0...3   */ 
    float motor_vibration;     /* offset 4...7   */
    float motor_temperature;  /* offset 8...11  */
    float motor_current;     /* offset 12...15 */
    float clima_humidity;      /* offset 16...19 */
    float clima_temperature;  /* offset 20...23 */
    int8_t motor_duty_pct;    /* offset 24      */
    uint8_t status_system;   /* offset 25      */
} PackageTelemetry_t;         /* tamanho total = 26 bytes */

_Static_assert(sizeof(PackageTelemetry_t) == 26, "Tamanho do pacote de telemetria diferente de 26 bytes");

#ifdef __cplusplus
}
#endif

#endif 