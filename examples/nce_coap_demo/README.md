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

## Device Controller
The Device Controller is an API that allows you to interact with devices integrated into the 1NCE API. You can use this API to send requests to devices, and the devices will respond accordingly. For more details you can visit our [DevHub](https://help.1nce.com/dev-hub/docs/1nce-os-device-controller)

### Sending a Request
To send a request to a specific device, you can use the following `curl` command:

```curl -X 'POST' 'https://api.1nce.com/management-api/v1/integrate/devices/<ICCID>/actions/COAP' \
-H 'accept: application/json' \
-H 'Authorization: Bearer <your Access Token >' \
-H 'Content-Type: application/json' \
-d '{
  "payload": "Data to send to the device",
  "payloadType": "STRING",
  "port": <NCE_RECV_PORT>,
  "path": "/example?param1=query_example1",
  "requestType": "POST",
  "requestMode": "SEND_NOW"
}'
```
Replace `<ICCID>` with the ICCID (International Mobile Subscriber Identity) of the target device and `<your Access Token>` with the authentication token from [Obtain Access Token](https://help.1nce.com/dev-hub/reference/postaccesstokenpost).

##### Requested Parameters

* `payload`: The data you want to send to the device. This should be provided as a string.
* `payloadType`: The type of the payload. In this example, it is set to "STRING".
* `port`: The port number on which the device will receive the request. In the code, this is defined as CONFIG_NCE_RECV_PORT.
* `path`: The path of the request. It can include query parameters as well. In this example, it is set to "/example?param1=query_example1".
* `requestType`: The type of request, such as "POST", "GET", etc.
* `requestMode`: The mode of the request. In this example, it is set to "SEND_NOW". There are other possible value like SEND_WHEN_ACTIVE.

#### Arduino Configuration

To handle the incoming request from the 1NCE API, the configuration of certain parameters needed
* `CONFIG_NCE_RECV_PORT`: This is the port number where your device will listen for incoming requests. It should match the port parameter used in the request (by default is 3000).

#### Arduino Output

When the Arduino application receives a request from the 1NCE API, it will produce output similar to the following:

```
Received a message of length '71'
	msg_id:           19344
	msg_code:         2
	content_format:   0
	payload_len:      26
	payload:          Data to send to the device
	options:
	URI: example
	QUERY: param1=query_example1
```

