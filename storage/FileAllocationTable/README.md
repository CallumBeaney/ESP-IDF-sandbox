Here using FAT we also utilize wear-levelling. This ensures that our MC to write to different sectors of memory to ensure their finite RW capacity is not exhausted.


### menuconfig
`components -> partition_table -> CSV files`  
Then append the .csv with an additional partition. Then, to set it on the chip,:  
``` 
idf.py menuconfig
  Partition Table
    1st setting: Partition Table (etcetc)
      Custom CSV
  Serial Flasher Config
    Flash Size
      4MB or whatever you need
```
Compilation issues specific to partitions.csv alias fix:
```
component config
  FAT Filesystem support
    Long filename support
```

