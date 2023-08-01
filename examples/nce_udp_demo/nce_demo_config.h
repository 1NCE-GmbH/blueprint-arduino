/* Define UDP Endpoint & Port */
#define NCE_UDP_ENDPOINT "udp.os.1nce.com"
#define NCE_UDP_PORT "4445"

/* Energy Saver Settings */
#define ENABLE_NCE_ENERGY_SAVER
/* Define Payload */
#ifdef ENABLE_NCE_ENERGY_SAVER
#define NCE_PAYLOAD_DATA_SIZE  10
#else
#define NCE_PAYLOAD "Hello from Arduino, this is a test message!"
#endif

/* Generic Demo Settings */
#define NCE_UDP_DATA_UPLOAD_FREQUENCY_SECONDS 20
