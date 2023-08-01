# 1NCE Arduino blueprint - UDP Demo

## Overview

1NCE Arduino UDP Demo allows customers to communicate with 1NCE endpoints via UDP Protocol, and it can send compressed payload using the Energy Saver feature. 


## Using 1NCE Energy saver
The demo can send optimized payload using 1NCE Energy saver. This feature is enabled by default with the following definition in `nce_demo_config.h`

```
#define ENABLE_NCE_ENERGY_SAVER
```

The energy saver template used in the demo can be found in `extras/template.json`
## Configuration options


The configuration options for UDP sample are:

`NCE_UDP_ENDPOINT` is set to 1NCE endpoint.

`NCE_UDP_PORT` is set by default to the 1NCE UDP endpoint port 4445.

`NCE_UDP_DATA_UPLOAD_FREQUENCY_SECONDS` the interval between UDP packets.

`NCE_PAYLOAD` Message to send to 1NCE IoT Integrator.

`NCE_PAYLOAD_DATA_SIZE` Used when 1NCE Energy Saver is enabled to define the payload data size of the translation template.
