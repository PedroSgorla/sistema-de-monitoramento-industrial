/** 
 * @file app_motor_ctrl.h
 * @brief API para controle e inicializacao do motor
 */

#ifndef APP_MOTOR_CTRL_H
#define APP_MOTOR_CTRL_H

#include <stdint.h>

void app_motor_ctrl_init();
void app_motor_ctrl_set_command(int8_t duty_pct);

#endif