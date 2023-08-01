#define NCE_ICCID    "<ICCID>"
#define LWM2M_ENDPOINT    "lwm2m.os.1nce.com"
/* DLTLS Settings */
#define LwM2M_ENABLE_DTLS
#ifdef LwM2M_ENABLE_DTLS
#define LWM2M_BOOTSTRAP_PORT "5684"
#define LWM2M_BOOTSTRAP_PSK    "<PSK>"
#else
#define LWM2M_BOOTSTRAP_PORT "5683"
#endif

/* LwM2M Settings */
#define LWM2M_BOOTSTRAP
#define LWM2M_CLIENT_MODE
#define LWM2M_OBJECT_SEND
#define LWM2M_SUPPORT_SENML_JSON
#define LWM2M_SUPPORT_TLV
#define LWM2M_SUPPORT_JSON
#define LWM2M_COAP_DEFAULT_BLOCK_SIZE 1024
#define LWM2M_VERSION_1_1
#define LWM2M_LITTLE_ENDIAN
#define LWM2M_SINGLE_SERVER_REGISTERATION

