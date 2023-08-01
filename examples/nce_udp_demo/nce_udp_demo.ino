#include <nce_iot_c_sdk.h>
#include <udp_interface_arduino.h>

#include "Arduino.h"
#include <GSM.h>
REDIRECT_STDOUT_TO(Serial);
#include "arduino_secrets.h"
#include "nce_demo_config.h"
char pin[] = SECRET_PIN;
char apn[] = SECRET_APN;
char username[] = SECRET_USERNAME;
char pass[] = SECRET_PASSWORD;

GSMUDP client;

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
  Serial.println("1NCE UDP Demo Started...");
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

  /* Send the UDP packet to 1NCE OS */
  int ret = client.begin(atoi(NCE_UDP_PORT));
  ret = client.beginPacket(NCE_UDP_ENDPOINT, atoi(NCE_UDP_PORT));
  ret = client.write((uint8_t const*)buffer, sizeof(buffer));
  printf("Sent %d Bytes \n", ret);
  ret = client.endPacket();

  delay(NCE_UDP_DATA_UPLOAD_FREQUENCY_SECONDS * 1000);
}