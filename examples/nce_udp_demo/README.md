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

## Device Controller
The Device Controller is an API that allows you to interact with devices integrated into the 1NCE API. You can use this API to send requests to devices, and the devices will respond accordingly. For more details you can visit our [DevHub](https://help.1nce.com/dev-hub/docs/1nce-os-device-controller)

### Sending a Request
To send a request to a specific device, you can use the following `curl` command:

#### COAP
```curl -X 'POST' 'https://api.1nce.com/management-api/v1/integrate/devices/<ICCID>/actions/UDP' \
-H 'accept: application/json' \
-H 'Authorization: Bearer <your Access Token >' \
-H 'Content-Type: application/json' \
-d '{
  "payload": "enable_sensor",
  "payloadType": "STRING",
  "port": 3000,
  "requestMode": "SEND_NOW"
}'
```
Replace `<ICCID>` with the ICCID (International Mobile Subscriber Identity) of the target device and `<your Access Token>` with the authentication token from [Obtain Access Token](https://help.1nce.com/dev-hub/reference/postaccesstokenpost).

##### Requested Parameters

* `payload`: The data you want to send to the device. This should be provided as a string.
* `payloadType`: The type of the payload. In this example, it is set to "STRING".
* `port`: The port number on which the device will receive the request. In the code, this is defined as CONFIG_NCE_RECV_PORT.
* `requestMode`: The mode of the request. In this example, it is set to "SEND_NOW". There are other possible value like SEND_WHEN_ACTIVE.

#### Arduino Configuration

To handle the incoming request from the 1NCE API, the configuration of certain parameters needed
* `CONFIG_NCE_RECV_PORT`: This is the port number where your device will listen for incoming requests. It should match the port parameter used in the request (by default is 3000).

#### Arduino Output

When the Arduino application receives a request from the 1NCE API, it will produce output similar to the following:

```
Received UDP packet:
enable_sensor
```