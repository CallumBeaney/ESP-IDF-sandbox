idf_component_register(
    SRCS
    "main.c"
    "NVS_KV_pairs/nvs.c"
    "SPIFFS/SPIFFS.c"
    INCLUDE_DIRS
    "."
    "NVS_KV_pairs"
    "SPIFFS"
    EMBED_TXTFILES
    "NVS_demoFiles/index.html"
    "NVS_demoFiles/sample.txt"
    EMBED_FILES
    "NVS_demoFiles/pinout.jpg"
)

# Remember: you can also use this make syntax:
# set(COMPONENT_SRCS "main.c")
# set(COMPONENT_ADD_INCLUDEDIRS ".")
# register_component()

spiffs_create_partition_image(SPIFFPtnNameInCSVConfig ../spiffs_dir FLASH_IN_PROJECT)
