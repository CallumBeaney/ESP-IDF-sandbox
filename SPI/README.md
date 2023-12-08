see [this](https://learnesp32.com/videos/i2c-spi-uart/i2c-introduction) video for using the tools for I2C

I2C things to remember
1. Each device has an address
2. Each device has one or more registers
3. Some registers can be read from others can be written too
4. Sometimes you first need to write to a register as a command before doing a read
5. Sometimes you need to read or write more then one byte at a time
6. The specifics on how to communicate to the components is defined in the datasheet of that component