# Describes files/headers inside project main. 
idf_component_register(
  SRCS "helpers/helpers.c" "main.c" 
  INCLUDE_DIRS "." "helpers" 
  REQUIRES "freertos" "esp_hw_support" "driver"
)
