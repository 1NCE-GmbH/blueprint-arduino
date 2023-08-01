# 1NCE Arduino blueprint - CoAP Demo

## Overview

1NCE Arduino CoAP Demo allows customers to establish a secure communication with 1NCE endpoints via CoAPs after receiving DTLS credentials from Device Authenticator using the SDK. It can also send compressed payload using the Energy Saver feature. 

## Secure Communication with DTLS using 1NCE SDK

By default, the demo uses 1NCE SDK to send a CoAP GET request to 1NCE OS Device Authenticator. The response is then processed by the SDK and the credentials are used to connect to 1NCE endpoint via CoAP with DTLS. 

 ## Using 1NCE Energy saver
The demo can send optimized payload using 1NCE Energy saver. This feature is enabled by default with the following definition in `nce_demo_config.h`

```
#define ENABLE_NCE_ENERGY_SAVER
```
The energy saver template used in the demo can be found in `extras/template.json`

 ## Unsecure CoAP Communication 

To test unsecure communication, disable the device authenticator by removing the following definition from `nce_demo_config.h`

```
#define ENABLE_NCE_DEVICE_AUTHENTICATOR
``` 

## Configuration options


The configuration options for CoAP sample are:

`NCE_COAP_ENDPOINT` is set to 1NCE endpoint.

`NCE_COAP_PORT` is set automatically based on security options (with/without DTLS).

`NCE_COAP_URI_QUERY` the URI Query option used to set the MQTT topic for 1NCE IoT integrator.

`NCE_COAP_DATA_UPLOAD_FREQUENCY_SECONDS` the interval between CoAP packets.

`NCE_PAYLOAD` Message to send to 1NCE IoT Integrator.

`NCE_PAYLOAD_DATA_SIZE` Used when 1NCE Energy Saver is enabled to define the payload data size of the translation template.
