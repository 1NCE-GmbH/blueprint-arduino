# 1NCE Arduino blueprint - LwM2M Demo

## Overview

1NCE Arduino LwM2M Demo allows customers to communicate with 1NCE endpoints via LwM2M Protocol. 

LwM2M Actions can be tested using the [Action API](https://help.1nce.com/dev-hub/reference/post_v1-devices-deviceid-actions). For example:

- To get the firmare update object info, send a `read` action to object `/5`.

## Configuration options

The configuration options for LwM2M sample are:

`NCE_ICCID` the ICCID of 1NCE SIM Card.

`LWM2M_ENDPOINT` is set to 1NCE endpoint.

DTLS is enabled by default. To use DTLS, bootstraping PSK should be defined in `LWM2M_BOOTSTRAP_PSK`. It can be configured while testing the LwM2M integration (From the Device integrator), or from the API [Create Pre-Shared Device Key](https://help.1nce.com/dev-hub/reference/post_v1-integrate-devices-deviceid-presharedkey). 


## Unsecure LwM2M Communication 

To test unsecure communication, disable the device authenticator by removing the following definition from `nce_demo_config.h`

```
#define LwM2M_ENABLE_DTLS
``` 
