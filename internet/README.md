Access points are devices that provide wireless connectivity to a network, stations are devices that connect to the network wirelessly, and clients are devices that are connected to the network and can communicate with other devices on the network.  
  
Many of these are quick n dirty, not for production. Just basic workflows. wifi_init.c and its associated wifi_connect functions are more professional level.  
All sections are in main dir.  

Note your top-level CMakeLists: `set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)`.  

See: `esp/esp-idf/examples/common_components/protocol_examples_common` for connection examples.