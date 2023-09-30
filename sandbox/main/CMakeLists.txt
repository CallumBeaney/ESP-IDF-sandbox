# Describes files/headers inside project main. 
idf_component_register(
  
  SRCS 
    "main.c"
    "freeRTOScore/timers/timers.c"
    "freeRTOScore/delay/delay.c"
    "freeRTOScore/twotasks/tasks.c"
    "synchronisationPrimitives/mutex/mutex.c"
    "synchronisationPrimitives/semaphore/sema.c"
    "synchronisationPrimitives/queue/queue.c"
    "synchronisationPrimitives/eventGroup/group.c"
  
  INCLUDE_DIRS 
    "." 
    "freeRTOScore/delay" 
    "freeRTOScore/twotasks"
    "freeRTOScore/timers"
    "synchronisationPrimitives/mutex"
    "synchronisationPrimitives/semaphore"
    "synchronisationPrimitives/queue"
    "synchronisationPrimitives/eventGroup"

  REQUIRES 
    "freertos"
    "esp_hw_support"
    "driver"
    "esp_timer"
)
