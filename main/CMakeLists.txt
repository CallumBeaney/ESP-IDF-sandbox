# Describes files/headers inside project main. 
idf_component_register(
  SRCS 
    "main.c"
    "helpers/helpers.c"
    "twotasks/tasks.c"
  
  INCLUDE_DIRS 
    "." 
    "helpers" 
    "twotasks"
  
  REQUIRES 
    "freertos"
    "esp_hw_support"
    "driver"
)
