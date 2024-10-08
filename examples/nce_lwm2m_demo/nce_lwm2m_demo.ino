#include <nce_iot_c_sdk.h>
#include <udp_interface_arduino.h>

extern "C" {
#include "src/wakaama/include/liblwm2m.h"
#include "src/wakaama/client/lwm2mclient.h"
}

#include "Arduino.h"
#include <GSM.h>
REDIRECT_STDOUT_TO(Serial);
#include "arduino_secrets.h"
#include "nce_demo_config.h"
char pin[] = SECRET_PIN;
char apn[] = SECRET_APN;
char username[] = SECRET_USERNAME;
char pass[] = SECRET_PASSWORD;
time_t current_time;

#ifdef LwM2M_ENABLE_DTLS
#include "TLSSocket.h"
#include "DTLSSocket.h"
#include <MbedClient.h>
#include <MbedServer.h>
#include <MbedSSLClient.h>
#include <MbedUdp.h>
#include <SocketHelpers.h>
#include "socket.h"
/* DTLS Definitions */
int DtlsPort = 5684;
DTLSSocket bootstrap_socket;
DTLSSocket _socket;
mbedtls_ssl_config config;
char lwm2m_psk[50];
char lwm2m_psk_id[50];
#endif

/*LwM2M Definitions*/
#define MAX_PACKET_SIZE 2048
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "35.158.0.92"
bool DEVICE_BOOTSTRAPPED = false;
#define OBJ_COUNT 5
lwm2m_object_t* objArray[OBJ_COUNT];
client_data_t data;
lwm2m_context_t* lwm2mH = NULL;
char localPort[] = "56830";
char server[] = LWM2M_ENDPOINT;
char name[] = NCE_ICCID;
int lifetime = 300;
int result = 0;
int opt;
bool bootstrapRequested = false;
bool serverPortChanged = false;
lwm2m_client_state_t previousState = STATE_INITIAL;

GSMUDP client;

time_t get_time(void) {
  return current_time;
}
int led_on_off_func(bool value){
  Serial.println("Led_ON_OFF");
  pinMode(LEDG, OUTPUT);
  if(value == true){
    digitalWrite(LEDG, LOW);
    return 1;
  }
  else{
    digitalWrite(LEDG, HIGH);
    return 0;
  }
}
int led_rgb_func(uint8_t red, uint8_t green, uint8_t blue){
  Serial.println("Led_Control_RGB");
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDR, OUTPUT);

  digitalWrite(LEDG, 255-green);
  digitalWrite(LEDR, 255-red);
  digitalWrite(LEDB, 255-blue);

  return 1;
}
/*LwM2M UDP Interface*/
/* Connect */
connection_t* connection_create(connection_t* connList,
                                int sock,
                                char* host,
                                char* port,
                                int addressFamily) {
  connection_t* connP = NULL;
  int ret;

#ifdef LwM2M_ENABLE_DTLS

  const int ciphersuite_list[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256, MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256, MBEDTLS_TLS_PSK_WITH_AES_128_CCM, MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8, 0 };
  /* Configure DTLS*/
  if (DEVICE_BOOTSTRAPPED) {
    ret = _socket.open(GSM.getNetwork());
    SocketAddress addr = SocketAddress(host, atoi(port));
    ret = GSM.getNetwork()->gethostbyname(host, &addr);
    ret = mbedtls_ssl_conf_psk(_socket.get_ssl_config(),
                               (const unsigned char*)&lwm2m_psk, strlen(lwm2m_psk), (const unsigned char*)&NCE_ICCID, strlen(NCE_ICCID));
    mbedtls_ssl_conf_ciphersuites(_socket.get_ssl_config(), ciphersuite_list);
    ret = mbedtls_ssl_set_hostname(_socket.get_ssl_context(), host);
    ret = _socket.connect(addr);
  } else {
    ret = bootstrap_socket.open(GSM.getNetwork());
    SocketAddress addr = SocketAddress(host, atoi(port));
    ret = GSM.getNetwork()->gethostbyname(host, &addr);
    ret = mbedtls_ssl_conf_psk(bootstrap_socket.get_ssl_config(),
                               (const unsigned char*)&LWM2M_BOOTSTRAP_PSK, strlen(LWM2M_BOOTSTRAP_PSK), (const unsigned char*)&NCE_ICCID, strlen(NCE_ICCID));
    mbedtls_ssl_conf_ciphersuites(bootstrap_socket.get_ssl_config(), ciphersuite_list);
    ret = mbedtls_ssl_set_hostname(bootstrap_socket.get_ssl_context(), host);
    ret = bootstrap_socket.connect(addr);
  }
  if (ret == 0) {
    connP = connection_new_incoming(connList, sock, host, sizeof(host));
  }

#else

  ret = client.begin(atoi(port));
  ret = client.beginPacket(host, atoi(port));
  if (ret == 1) {
    connP = connection_new_incoming(connList, sock, host, sizeof(host));
  }

#endif
  return connP;
}

/* Send */
int connection_send(connection_t* connP,
                    uint8_t* buffer,
                    size_t length) {
  int nbSent;
  size_t offset;
  int result;

#ifdef LwM2M_ENABLE_DTLS

  if (DEVICE_BOOTSTRAPPED) {
    nbSent = _socket.send(buffer, length);

  } else {
    nbSent = bootstrap_socket.send(buffer, length);
  }

  if (nbSent < 1) {
    return -1;
  }


#else

  nbSent = client.write((uint8_t const*)buffer, length);
  result = client.endPacket();
  if (result != 1) {
    return -1;
  }

#endif
  return 0;
}

/* Disconnect */
void connection_close(void) {
#ifdef LwM2M_ENABLE_DTLS
  int ret;
  if (DEVICE_BOOTSTRAPPED) {
    ret = _socket.close();

  } else {
    ret = bootstrap_socket.close();
  }
#else
  client.stop();
#endif
}

/* Receive */
void Wakaama_Recv(client_data_t data, int socketHandle, lwm2m_context_t* lwm2mH, uint8_t* buffer, void* fromSessionH) {
  int ret;
  for (int i = 0; i <= 10; i++) {

#ifdef LwM2M_ENABLE_DTLS

    if (DEVICE_BOOTSTRAPPED) {
      _socket.set_timeout(200);
      ret = _socket.recv(buffer, MAX_PACKET_SIZE);
    } else {
      bootstrap_socket.set_timeout(200);
      ret = bootstrap_socket.recv(buffer, MAX_PACKET_SIZE);
    }

#else
    ret = client.parsePacket();
#endif

    if (ret > 0) {
      printf("Received packet of size %d \n", ret);
      client.read((unsigned char*)buffer, ret);
      lwm2m_handle_packet(lwm2mH, buffer, ret, fromSessionH);
      break;
    }
    delay(100);
  }

  return;
}

#ifdef LwM2M_ENABLE_DTLS
/* PSK Storage */
void store_credentials(void) {

  int keyLen;
  char* key;
  int idLen;
  char* id;

  key = security_get_secret_key(lwm2mH, lwm2mH->objectList, 0, &keyLen);
  memset(lwm2m_psk, (int8_t)'\0', sizeof(lwm2m_psk));
  memcpy(lwm2m_psk, key, keyLen);
  lwm2m_free(key);

  id = security_get_public_id(lwm2mH, lwm2mH->objectList, 0, &idLen);
  memset(lwm2m_psk_id, (int8_t)'\0', sizeof(lwm2m_psk_id));
  memcpy(lwm2m_psk_id, id, idLen);
  lwm2m_free(id);

  char* uri;
  char uriBuf[50];
  uri = security_get_uri(lwm2mH, lwm2mH->objectList, 0, uriBuf, 50);
  printf(" LwM2M Server: %s  \n", uriBuf);
  DEVICE_BOOTSTRAPPED = true;
}
#endif

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
  Serial.println("1NCE LwM2M Demo Started...");

  /*Initialize LwM2MObjects*/
  char serverUri[50];
  sprintf(serverUri, "coap://%s:%s", LWM2M_ENDPOINT, LWM2M_BOOTSTRAP_PORT);

  int serverId = 0;
  lwm2m_object_t* securityObj;

  #ifdef LwM2M_ENABLE_DTLS
  objArray[0] = get_security_object(serverId, serverUri, (char*)NCE_ICCID, (char*)LWM2M_BOOTSTRAP_PSK, strlen(NCE_ICCID), true);
  #else
  objArray[0] = get_security_object(serverId, serverUri, NULL, NULL, 0, true);
  #endif

  if (NULL == objArray[0]) {
    printf("Failed to create security object\r\n");
  }
  data.securityObjP = objArray[0];

  objArray[1] = get_server_object(serverId, "U", lifetime, false);

  if (NULL == objArray[1]) {
    printf("Failed to create server object\r\n");
  }

  objArray[2] = get_object_device();
  if (NULL == objArray[2]) {
    printf("Failed to create Device object\r\n");
  }

  objArray[3] = get_object_firmware();
  if (NULL == objArray[3]) {
    printf("Failed to create Firmware object\r\n");
  }

  objArray[4] = get_test_object();
  if (NULL == objArray[4])
  {
      printf("Failed to create Led object\r\n");
  }

  lwm2mH = lwm2m_init(&data);

  if (NULL == lwm2mH) {
    printf("lwm2m_init() failed\r\n");
  }

  result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
  if (result != 0) {
    printf("lwm2m_configure() failed: 0x%X\r\n", result);
  }

  /*define Network interface*/
  network_interface_t* networkInterface = (network_interface_t*)malloc(sizeof(network_interface_t));
  networkInterface->connection_create = connection_create;
  networkInterface->connection_send = connection_send;
  networkInterface->connection_close = connection_close;
  lwm2mH->networkInterface = networkInterface;

  /*define get time function*/
  time_function_t get_time_fun = &get_time;
  lwm2mH->lwm2m_gettime = get_time_fun;

  /* Initialize value changed callback. */
  init_value_change(lwm2mH);
  printf("LWM2M Client \"%s\" started on port %s\r\n", name, localPort);
  printf("> ");

  /* define led_on_off function*/
  led_on_off get_led_fun = &led_on_off_func;
  lwm2mH->lwm2m_led = get_led_fun;
  
  /* define led_rgb function*/
  led_rgb get_led_rgb_fun = &led_rgb_func;
  lwm2mH->lwm2m_set_led_color = get_led_rgb_fun;
#define BACKUP_OBJECT_COUNT 2
  lwm2m_object_t* backupObjectArray[BACKUP_OBJECT_COUNT];
  delay(5000);
}

void loop() {

  current_time = millis() / (time_t)1000;
  get_time();
  time_t tv_sec = 60;

  /* Check current status and perform any pending operations */
  result = lwm2m_step(lwm2mH, &(tv_sec));
  printf(" -> State: ");

  switch (lwm2mH->state) {
    case STATE_INITIAL:
      printf("STATE_INITIAL\r\n");
      break;
    case STATE_BOOTSTRAP_REQUIRED:
      printf("STATE_BOOTSTRAP_REQUIRED\r\n");
      break;
    case STATE_BOOTSTRAPPING:
      printf("STATE_BOOTSTRAPPING\r\n");
      break;
    case STATE_REGISTER_REQUIRED:
      printf("STATE_REGISTER_REQUIRED\r\n");

#ifdef LwM2M_ENABLE_DTLS
      if (!DEVICE_BOOTSTRAPPED) {
        store_credentials();
      }
#endif

      break;
    case STATE_REGISTERING:
      printf("STATE_REGISTERING\r\n");
      break;
    case STATE_READY:
      printf("STATE_READY\r\n");
      break;
    default:
      printf("Unknown...\r\n");
      break;
  }

  if (result != 0) {
    printf("lwm2m_step() failed: 0x%X\r\n", result);
    if (previousState == STATE_BOOTSTRAPPING) {
      lwm2mH->state = STATE_INITIAL;
    }
  }

#ifdef LWM2M_BOOTSTRAP
  update_bootstrap_info(&previousState, lwm2mH);
#endif

  uint8_t buffer[MAX_PACKET_SIZE];
  memset(buffer, '\0', MAX_PACKET_SIZE);

  if (lwm2mH->serverList != NULL) {
    /* Receive data from LwM2M server */
    connection_t* connP = (connection_t*)lwm2mH->serverList->sessionH;
    if (connP != NULL) {
      Wakaama_Recv(data, connP->sock, lwm2mH, buffer, connP);
    }
  } else {
    /* Receive data from bootsrap server */
    connection_t* connP = (connection_t*)lwm2mH->bootstrapServerList->sessionH;
    if (lwm2mH->bootstrapServerList->status != STATE_BS_FINISHED && connP != NULL) {
      Wakaama_Recv(data, connP->sock, lwm2mH, buffer, connP);
    }
  }

  delay(200);
}