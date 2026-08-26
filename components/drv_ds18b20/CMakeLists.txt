idf_component_register(SRCS "drv_ds18b20.c"
                        INCLUDE_DIRS "include"
                        REQUIRES bsp_motor_node onewire_bus ds18b20)