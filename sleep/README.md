See the Datasheets for finer details.


### SLEEP DEETS  
| full power  | light sleep | deep sleep   |
|-------------|-------------|--------------|
| peripherals |             |              |
| bluetooth   |             |              |
| wifi        |             |              |
| cores       | cores       |              |
| coprocessor | coprocessor | coprocessor* |
| rtc         | rtc         | rtc          |
  
 ** The coprocessor is a sort of mini-CPU programmable only in ASM. It can read the I2C Bus and listen to A2D conversions + awake the chip.  


### WAKEUP MODES
| light sleep         | deep sleep          |
|---------------------|---------------------|
| Timer               | timer               |
| touch pad           | touch pad           |
| external wakeup     | external wakeup*    |
| ULP** Coprocessor ADC | ULP Coprocessor ADC |
| ULP Coprocessor ADC | ULP Coprocessor ADC |
| GPIO wakeup         |                     |
| UART wakeup         |                     |

*i.e. wakeup from a HIGH/LOW GPIO pin change
** ultra low power