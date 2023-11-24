idf_component_register(
  SRCS 
  "main.c"
  "interrupts/interrupts.c"
  "ledc_pwm/ledc.c"
  
  INCLUDE_DIRS 
  "."
  "interrupts"
  "ledc_pwm"
  )

