# Describes files/headers inside project main. 
idf_component_register(
  
  SRCS 
    "main.c"
    "timeHandling/timers/timers.c"
    "timeHandling/delay/delay.c"
    "synchronisationPrimitives/twotasks/tasks.c"
    "synchronisationPrimitives/mutex/mutex.c"
    "synchronisationPrimitives/semaphore/sema.c"
    "synchronisationPrimitives/queue/queue.c"
    "synchronisationPrimitives/eventGroup/group.c"
  
  INCLUDE_DIRS 
    "." 
    "timeHandling/delay" 
    "timeHandling/timers"
    "synchronisationPrimitives/twotasks"
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
