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
  
### RE: ULP

The ULP (Ultra Low Power) coprocessor is a small, lightweight processor integrated into the ESP32 microcontroller. It is designed to handle simple tasks, such as monitoring sensors, while the main CPU of the ESP32 is in deep sleep mode to conserve power. The ULP coprocessor is a finite state machine (FSM), meaning that it executes a sequence of instructions based on its current state and the input it receives.  

The ULP coprocessor has several advantages over using the main CPU for low-power tasks:  

- Lower power consumption: The ULP coprocessor is much simpler than the main CPU and consumes significantly less power when running.
- Wake-on-event capability: The ULP coprocessor can be configured to wake up the main CPU from deep sleep only when certain events occur, such as a change in sensor readings.
- Independent operation: The ULP coprocessor can continue to operate even when the main CPU is in deep sleep, making it ideal for applications that require continuous monitoring.

**Usecases**:

- Monitoring a battery voltage to prevent over-discharge
- Detecting motion or vibration
- Performing periodic measurements of environmental conditions
- Generating wake-up events for the main CPU
