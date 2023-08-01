#include <nce_iot_c_sdk.h>
#include <udp_interface_arduino.h>

#include "Arduino.h"
#include <GSM.h>
REDIRECT_STDOUT_TO(Serial);
#include "arduino_secrets.h"
#include "nce_demo_config.h"
#include "sn_coap_protocol.h"
#include "sn_coap_header.h"
char pin[] = SECRET_PIN;
char apn[] = SECRET_APN;
char username[] = SECRET_USERNAME;
char pass[] = SECRET_PASSWORD;

#ifdef ENABLE_NCE_DEVICE_AUTHENTICATOR
/* DTLS Definitions */
#include "DTLSSocket.h"
int DtlsPort = 5684;
DTLSSocket _socket;
mbedtls_ssl_config config;

/* 1NCE SDK UDP Interface & DTLS Key */
struct OSNetwork xOSNetwork = { 0 };
os_network_ops_t osNetwork = {
  .os_socket = &xOSNetwork,
  .nce_os_udp_connect = nce_os_udp_connect,
  .nce_os_udp_send = nce_os_udp_send,
  .nce_os_udp_recv = nce_os_udp_recv,
  .nce_os_udp_disconnect = nce_os_udp_disconnect
};
DtlsKey_t nceKey = { 0 };
#else
GSMUDP client;
#endif

/* CoAP Definitions */
struct coap_s* coapHandle;
coap_version_e coapVersion = COAP_VERSION_1;
sn_coap_hdr_s* coap_res_ptr = (sn_coap_hdr_s*)calloc(sizeof(sn_coap_hdr_s), 1);
sn_coap_options_list_s* optn = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
void* coap_malloc(uint16_t size) {
  return malloc(size);
}

/* CoAP interface */
void coap_free(void* addr) {
  free(addr);
}

uint8_t coap_tx_cb(uint8_t* a, uint16_t b, sn_nsdl_addr_s* c, void* d) {
  printf("coap tx cb\n");
  return 0;
}

int8_t coap_rx_cb(sn_coap_hdr_s* a, sn_nsdl_addr_s* b, void* c) {
  printf("coap rx cb\n");
  return 0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Starting Carrier Network registration");
  /* Configure the Radio Access Technology (RAT) and the Band */
  if (!GSM.begin(pin, apn, username, pass, CATM1, BAND_3)) {
    Serial.println("The board was not able to register to the network...");
    // do nothing forevermore:
    while (1)
      ;
  }
  /* Initialize CoAP library */
  coapHandle = sn_coap_protocol_init(&coap_malloc, &coap_free, &coap_tx_cb, &coap_rx_cb);
  coap_res_ptr->msg_id = 0;

  Serial.println("1NCE CoAP Demo Started...");

#ifdef ENABLE_NCE_DEVICE_AUTHENTICATOR

  /* Get DTLS Credentials using 1NCE SDK */
  int result = os_auth(&osNetwork, &nceKey);

  /* Create & Configure DTLS Socket */
  int sock = _socket.open(GSM.getNetwork());
  int ret = mbedtls_ssl_conf_psk(_socket.get_ssl_config(),
                                 (const unsigned char*)&nceKey.Psk, strlen(nceKey.Psk), (const unsigned char*)&nceKey.PskIdentity, strlen(nceKey.PskIdentity));
  const int ciphersuite_list[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256, MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256, MBEDTLS_TLS_PSK_WITH_AES_128_CCM, MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8, 0 };
  mbedtls_ssl_conf_ciphersuites(_socket.get_ssl_config(), ciphersuite_list);
  ret = mbedtls_ssl_set_hostname(_socket.get_ssl_context(), NCE_COAP_ENDPOINT);
  SocketAddress addr = SocketAddress(NCE_COAP_ENDPOINT, atoi(NCE_COAP_PORT));
  ret = GSM.getNetwork()->gethostbyname(NCE_COAP_ENDPOINT, &addr);

  /* Establish DTLS Connection */
  ret = _socket.connect(addr);
  if (ret == 0) {
    printf("DTLS connection is established \n");
  } else {
    printf("DTLS connection failed \n");
  }
#endif

}

void loop() {


/* If Energy saver is  enabled, send the optimized payload. Otherwise, send the default test payload */
#if defined(ENABLE_NCE_ENERGY_SAVER)
  char buffer[NCE_PAYLOAD_DATA_SIZE];
  int err;
  /* Define the variables according to the Energy saver template */
  Element2byte_gen_t battery_level;
  battery_level.type = E_INTEGER;
  battery_level.value.i = 99;
  battery_level.template_length = 1;
  Element2byte_gen_t signal_strength;
  signal_strength.type = E_INTEGER;
  signal_strength.value.i = 84;
  signal_strength.template_length = 1;
  Element2byte_gen_t software_version;
  char version[] = "2.2.1";
  software_version.type = E_STRING;
  memcpy(software_version.value.s, version, sizeof(version));
  software_version.template_length = 5;
  /* Create optimized payload */
  err = os_energy_save(buffer, 1, 3, battery_level, signal_strength, software_version);
  if (err < 0) {
    printf("Failed to create payload, %d\n", errno);
    return;
  }
#else
  char buffer[] = NCE_PAYLOAD;
#endif

  /* Build CoAP Packet */
  const char* uri_query_ptr = NCE_COAP_URI_QUERY;
  const char* coap_uri_host = NCE_COAP_ENDPOINT;
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_POST;
  coap_res_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->payload_len = strlen(buffer);
  coap_res_ptr->payload_ptr = (uint8_t*)buffer;
  coap_res_ptr->options_list_ptr = optn;
  optn->uri_query_ptr = (uint8_t*)uri_query_ptr;
  optn->uri_query_len = strlen(uri_query_ptr);
  optn->block1 = -1;
  optn->block2 = -1;
  coap_res_ptr->msg_id = coap_res_ptr->msg_id + 1;
  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t* message_ptr = (uint8_t*)malloc(message_len);
  sn_coap_builder(message_ptr, coap_res_ptr);


  /* Send the CoAP packet to 1NCE OS */
  #ifdef ENABLE_NCE_DEVICE_AUTHENTICATOR
  int ret = _socket.send(message_ptr, message_len);
  #else
  int ret = client.begin(atoi(NCE_COAP_PORT));
  ret = client.beginPacket(NCE_COAP_ENDPOINT, atoi(NCE_COAP_PORT));
  ret = client.write((uint8_t const*)message_ptr, message_len);
  ret = client.endPacket();
  #endif

  printf("Sent %d Bytes \n", ret);

  delay(NCE_COAP_DATA_UPLOAD_FREQUENCY_SECONDS * 1000);
}