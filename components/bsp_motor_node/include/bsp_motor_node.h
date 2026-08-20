/**
 * @file bsp_motor_node.h
 * @brief Camada do firmware para setar os pinos do MCU(Hardcoded) 
 */

#ifndef BSP_MOTOR_NODE_H
#define BSP_MOTOR_NODE_H

/* ---- I2C - MPU6050 ----- */
#define BSP_I2C_SDA_GPIO  8
#define BSP_I2C_SCL_GPIO  9

/* ---- SPI - cartao SD(Logger) ----- */
#define BSP_SD_SPI_SCK_GPIO   12
#define BSP_SD_SPI_MOSI_GPIO  11
#define BSP_SD_SPI_MISO_GPIO  13
#define BSP_SD_SPI_CS_GPIO    14

/* ---- 1-Wire RMT - DS18B20 ----- */
#define BSP_DS18B20_DATA_GPIO  10

/* ---- ADC1 - ACS712 ----- */
#define BSP_ADC_CORRENTE_GPIO  1

/* ---- CAN/TWAI - SN65HVD230 ----- */
#define BSP_CAN_TX_GPIO  4
#define BSP_CAN_RX_GPIO  5

/* ---- Ponte H - DRV8833 ----- */
#define BSP_MOTOR_PWM_GPIO    6 
#define BSP_MOTOR_IN1_GPIO    7
#define BSP_MOTOR_IN2_GPIO   15
#define BSP_MOTOR_STBY_GPIO  16

/* ---- Buzzer ----- */
#define BSP_BUZZER_GPIO  17

#endif 