idf_component_register(SRCS "app_motor_ctrl.c"
                       INCLUDE_DIRS "include" 
                       REQUIRES bsp_motor_node driver)