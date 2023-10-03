# Memory

## DRAM and the Heap: instruction RAM & program-based commands
Got this result from main.c
```
I (322) Memory: xPortGetFreeHeapSize 387416 = DRAM
I (332) Memory: DRAM             387416
I (332) Memory: IRam             0
```
BARD: 'The ESP32-S3-WROOM-1 does have IRAM, but it is a limited resource. The linker script for the ESP32-S3 reserves 360,000 bytes for IRAM, but some of this space is used by the system and by the ESP-IDF libraries.

To reduce IRAM usage: 
- Avoid using global variables.
- Use static variables whenever possible.
- Avoid using large arrays.
- Use functions instead of macros.
- Disable compiler optimizations if necessary.
  
#### Size & Static IRAM
if run `idf.py size`:

```
Total sizes:
Used static IRAM:   55850 bytes ( 306390 remain, 15.4% used)
      .text size:   54823 bytes
   .vectors size:    1027 bytes
Used stat D/IRAM:   13180 bytes ( 332676 remain, 3.8% used)
      .data size:   11028 bytes
      .bss  size:    2152 bytes
Used Flash size :  158843 bytes
           .text:  112719 bytes
         .rodata:   45868 bytes
Total image size:  225721 bytes (.bin may be padded larger)
```
`.dram size` is the amount of memory initialized that has an initial value or is assigned at initialisation. .bss is variables not initialised. If you do e.g. `int p;` that will be put on the `bss` stack.  

#### size-files
Imagine you declare two global variables and don't use them:
```
int data = 3;
char bss[100];
```
...the compiler won't register them, and the size report will be the same as if they weren't there.  
if run `idf.py size-files` 
```
Object File DRAM .data & 0.bss IRAM0 .text & 0.vectors ram_st_total Flash .text & .rodata & .appdesc flash_total
main.c.obj          0       0           0           0            0         158       151          0         309
...
```
If you now include a `printf("%d / %s", data, bss);` that uses them (even just printing their unassigned selves out), 
```
Object File DRAM .data & 0.bss IRAM0 .text & 0.vectors ram_st_total Flash .text & .rodata & .appdesc flash_total
main.c.obj          4     100         
...
```

## PSRAM
PSRAM cannot be used for task stack memory.  

```
`idf.py menuconfig` -> component config -> esp32 specific -> support for external SPI-connected RAM --->
```
```
[*] Initialize SPI RAM when booting the ESP32
[*] SPI RAM access method --->
      If you just choose `Integrate RAM into ESP32 memory map` you will have to do manual memory management; `make RAM allocatable using malloc()` preferable.
[*] Enable workaround for bug in SPI RAM cache for Rev1 ESP32S --->
      This consumes IRAM.
      If have "REVISION 1" of ESP32S, must select; if Revision 0, cannot use PSRAM. 
      If have Revision 2 chip, no necessity.
[*] Try to allocate memories of wiff and LWIP in SPIRAM firstly --->
      Passes wifi etc config data through PSRAM. Recommended.  
[*] Allow bss segment placed in external memory
```