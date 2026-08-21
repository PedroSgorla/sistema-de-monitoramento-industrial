/** 
 * @file app_motor_ctrl.h
 * @brief API para controle e inicializacao do motor
 */

#ifndef APP_MOTOR_CTRL_H
#define APP_MOTOR_CTRL_H

#include <stdint.h>

/**
 * @brief Inicializa o controle do motor.
 * Configura o periférico LEDC para gerar PWM e inicializa os pinos GPIO necessários.
 */
void app_motor_ctrl_init(void);

/**
 * @brief Define o sentido e a velocidade do motor.
 * 
 * Extrai o sentido de rotação(magnitude) e o duty cycle a partir do valor fornecido.
 * Se a opção de sentido for diferente do atual, o motor é desligado, o sentido é alterado e o motor é religado.
 * 
 * @param duty_pct Percentual de duty cycle para o motor, variando de -100 a 100.
 *                 Valores positivos indicam rotação no sentido horário.
 *                 Valores negativos indicam rotação no sentido anti-horário.
 *                 Valores acima de 100 ou abaixo de -100 serão ajustados para os limites.
 * 
 * @note O motor será desligado momentaneamente ao alterar o sentido de rotação para evitar danos.
 *       Esta função não é Thread-Safe.
 */
void app_motor_ctrl_set_command(int8_t duty_pct);

#endif