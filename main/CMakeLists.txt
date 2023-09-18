# Describes files/headers inside project main. 
idf_component_register(
  
  SRCS 
    "main.c"
    "delay/delay.c"
    "twotasks/tasks.c"
    "mutex/mutex.c"
    "semaphore/sema.c"
    "queue/queue.c"
    "eventGroup/group.c"
  
  INCLUDE_DIRS 
    "." 
    "delay" 
    "twotasks"
    "mutex"
    "semaphore"
    "queue"
    "eventGroup"

  REQUIRES 
    "freertos"
    "esp_hw_support"
    "driver"
)
