/* Device Authenticator Settings */
#define ENABLE_NCE_DEVICE_AUTHENTICATOR
/* Define CoAP Endpoint & Port */
#define NCE_COAP_ENDPOINT "coap.os.1nce.com"
#ifdef ENABLE_NCE_DEVICE_AUTHENTICATOR
#define NCE_COAP_PORT "5684"
#else
#define NCE_COAP_PORT "5683"
#endif

/* Energy Saver Settings */
#define ENABLE_NCE_ENERGY_SAVER
/* Define Payload */
#ifdef ENABLE_NCE_ENERGY_SAVER
#define NCE_PAYLOAD_DATA_SIZE 10
#else
#define NCE_PAYLOAD "Hello from Arduino, this is a test message!"
#endif

/* Generic Demo Settings */
#define NCE_COAP_DATA_UPLOAD_FREQUENCY_SECONDS 20
#define NCE_COAP_URI_QUERY "t=test"


