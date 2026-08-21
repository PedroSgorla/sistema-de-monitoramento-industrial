/**
 * @file protocolo_telemetria.h
 * @brief Definição do protocolo de telemetria para comunicação CAN.
 */
#ifndef PROTOCOLO_TELEMETRIA_H
#define PROTOCOLO_TELEMETRIA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status do sistema(armazenamento de um byte no pacote ) ---- */
enum {
    STATUS_OK = 0,
    STATUS_ATIVO = 1,
    STATUS_EMERGENCIA = 2,
    STATUS_FALHA_COMUNICACAO_CAN = 3,
};

/* ---- IDs CAN ---- */
enum {
    CAN_ID_EMERGENCIA = 0x001,
    CAN_ID_HEARTBEAT = 0x002,
    CAN_ID_COMANDO_VELOCIDADE = 0x010,
    CAN_ID_PARADA_LENTA = 0x011,
    CAN_ID_TELEMETRIA = 0x012,
};

/* ---- Contrato bytes compartilhado pelos 3 nós ---- */
typedef struct __attribute__((packed)) {
    uint32_t pacote_id;       /* offset 0...3   */ 
    float motor_vibracao;     /* offset 4...7   */
    float motor_temperatura;  /* offset 8...11  */
    float motor_corrente;     /* offset 12...15 */
    float clima_umidade;      /* offset 16...19 */
    float clima_temperatura;  /* offset 20...23 */
    int8_t motor_duty_pct;    /* offset 24      */
    uint8_t status_sistema;   /* offset 25      */
} PacoteTelemetria_t;         /* tamanho total = 26 bytes */

_Static_assert(sizeof(PacoteTelemetria_t) == 26, "Tamanho do pacote de telemetria diferente de 26 bytes");

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOLO_TELEMETRIA_H */