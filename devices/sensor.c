/*
 * Copyright (C) 2019
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3.0. See the file LICENSE in the top level
 * directory for more details.
 *
 * See AUTHORS.md for complete list of NDN IOT PKG authors and contributors.
 */
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ndn-lite.h>
#include "ndn-lite/encode/name.h"
#include "ndn-lite/encode/data.h"
#include "ndn-lite/encode/interest.h"
#include "ndn-lite/app-support/service-discovery.h"
#include "ndn-lite/app-support/access-control.h"
#include "ndn-lite/app-support/security-bootstrapping.h"
#include "ndn-lite/app-support/ndn-sig-verifier.h"
#include "ndn-lite/app-support/pub-sub.h"
#include "ndn-lite/encode/key-storage.h"
#include "ndn-lite/encode/ndn-rule-storage.h"
#include "mockhw.h"

//make IDE happy
#ifndef STATE_NAME
#define STATE_NAME has-smoke
#define DEVICE_NAME smoke-detector
#define SERVICE_ID 51
#define REFRESH_PERIOD 500
#endif
//code generation macros
#define str(s) #s
#define xstr(s) str(s)
#define STATE_NAME_GEN xstr(STATE_NAME)
#define DEVICE_NAME_GEN xstr(DEVICE_NAME)

// DEVICE manufacture-created private key
uint8_t secp256r1_prv_key_bytes[32] = {0};

// HERE TO SET pre-shared public key
uint8_t secp256r1_pub_key_bytes[64] = {0};

//HERE TO SET pre-shared secrets
uint8_t hmac_key_bytes[16] = {0};

// Device identifer
char device_identifier[30];
size_t device_len;

// Face Declare
// ndn_udp_face_t *face;
ndn_unix_face_t *face;
// Buf used in this program
uint8_t buf[4096];
// Wether the program is running or not
bool running;

//static ndn_trust_schema_rule_t same_room;
//static ndn_trust_schema_rule_t controller_only;

int load_bootstrapping_info()
{
  FILE *fp;
  char buf[255];
  char *buf_ptr;
  fp = fopen("sensor.txt", "r");
  if (fp == NULL)
    exit(1);
  size_t i = 0;
  for (size_t lineindex = 0; lineindex < 4; lineindex++)
  {
    memset(buf, 0, sizeof(buf));
    buf_ptr = buf;
    fgets(buf, sizeof(buf), fp);
    if (lineindex == 0)
    {
      for (i = 0; i < 32; i++)
      {
        sscanf(buf_ptr, "%2hhx", &secp256r1_prv_key_bytes[i]);
        buf_ptr += 2;
      }
    }
    else if (lineindex == 1)
    {
      buf[strlen(buf) - 1] = '\0';
      strcpy(device_identifier, buf);
    }
    else if (lineindex == 2)
    {
      for (i = 0; i < 64; i++)
      {
        sscanf(buf_ptr, "%2hhx", &secp256r1_pub_key_bytes[i]);
        buf_ptr += 2;
      }
    }
    else
    {
      for (i = 0; i < 16; i++)
      {
        sscanf(buf_ptr, "%2hhx", &hmac_key_bytes[i]);
        buf_ptr += 2;
      }
    }
  }
  fclose(fp);

  // prv key
  printf("Pre-installed ECC Private Key:");
  for (int i = 0; i < 32; i++)
  {
    printf("%02X", secp256r1_prv_key_bytes[i]);
  }
  printf("\nPre-installed Device Identifier: ");
  // device id
  printf("%s\nPre-installed ECC Pub Key: ", device_identifier);
  // pub key
  for (int i = 0; i < 64; i++)
  {
    printf("%02X", secp256r1_pub_key_bytes[i]);
  }
  printf("\nPre-installed Shared Secret: ");
  // hmac key
  for (int i = 0; i < 16; i++)
  {
    printf("%02X", hmac_key_bytes[i]);
  }
  printf("\n");
  return 0;
}

void periodic_publish(size_t param_size, uint8_t *param_value)
{
  static ndn_time_ms_t last;
  int value = readFromDevice(DEVICE_NAME_GEN, STATE_NAME_GEN);
  //printf("value of sensor output: %d\n", value);
  ps_event_t event = {
      .data_id = STATE_NAME_GEN,
      .data_id_len = strlen(STATE_NAME_GEN),
      .payload = &value,
      .payload_len = 1};

//publish every defined time
  if (ndn_time_now_ms() - last >= REFRESH_PERIOD)
  {
    ps_publish_content(SERVICE_ID, &event);
    last = ndn_time_now_ms();
  }
  ndn_msgqueue_post(NULL, periodic_publish, 0, NULL);
}

void after_bootstrapping()
{
  writeToDevice(DEVICE_NAME_GEN, STATE_NAME_GEN, 0);
  periodic_publish(0, NULL);
}

void SignalHandler(int signum)
{
  running = false;
}

int main(int argc, char *argv[])
{
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);
  signal(SIGQUIT, SignalHandler);

  // PARSE COMMAND LINE PARAMETERS
  int ret = NDN_SUCCESS;
  if ((ret = load_bootstrapping_info()) != 0)
  {
    return ret;
  }
  ndn_lite_startup();

  // CREAT A MULTICAST FACE
  face = ndn_unix_face_construct(NDN_NFD_DEFAULT_ADDR, true);
  // face = ndn_udp_unicast_face_construct(INADDR_ANY, htons((uint16_t) 2000), inet_addr("224.0.23.170"), htons((uint16_t) 56363));
  // in_port_t multicast_port = htons((uint16_t) 56363);
  // in_addr_t multicast_ip = inet_addr("224.0.23.170");
  // face = ndn_udp_multicast_face_construct(INADDR_ANY, multicast_ip, multicast_port);

  // LOAD SERVICES PROVIDED BY SELF DEVICE
  uint8_t capability[1];
  capability[0] = SERVICE_ID;

  // SET UP SERVICE DISCOVERY
  sd_add_or_update_self_service(SERVICE_ID, true, 1); // state code 1 means normal
  ndn_ac_register_encryption_key_request(SERVICE_ID);
  //ndn_ac_register_access_request(NDN_SD_LED);

  // START BOOTSTRAPPING
  ndn_bootstrapping_info_t booststrapping_info = {
      .pre_installed_prv_key_bytes = secp256r1_prv_key_bytes,
      .pre_installed_pub_key_bytes = secp256r1_pub_key_bytes,
      .pre_shared_hmac_key_bytes = hmac_key_bytes,
  };
  ndn_device_info_t device_info = {
      .device_identifier = device_identifier,
      .service_list = capability,
      .service_list_size = sizeof(capability),
  };
  ndn_security_bootstrapping(&face->intf, &booststrapping_info, &device_info, after_bootstrapping);

  // START MAIN LOOP
  running = true;
  while (running)
  {
    ndn_forwarder_process();
    usleep(1000);
  }

  // DESTROY FACE
  ndn_face_destroy(&face->intf);
  return 0;
}
