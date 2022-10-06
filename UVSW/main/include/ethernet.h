/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "top.h"

#define PORT    55603
#define addr_family AF_INET
#define ip_protocol IPPROTO_IP




#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
#define EXAMPLE_INTERFACE get_example_netif()
#endif

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
#define EXAMPLE_INTERFACE get_example_netif()
#endif

#if !defined (CONFIG_EXAMPLE_CONNECT_ETHERNET) && !defined (CONFIG_EXAMPLE_CONNECT_WIFI)
// This is useful for some tests which do not need a network connection
#define EXAMPLE_INTERFACE NULL
#endif

/**
 * @brief Configure Wi-Fi or Ethernet, connect, wait for IP
 *
 * This all-in-one helper function is used in protocols examples to
 * reduce the amount of boilerplate in the example.
 *
 * It is not intended to be used in real world applications.
 * See examples under examples/wifi/getting_started/ and examples/ethernet/
 * for more complete Wi-Fi or Ethernet initialization code.
 *
 * Read "Establishing Wi-Fi or Ethernet Connection" section in
 * examples/protocols/README.md for more information about this function.
 *
 * @return ESP_OK on successful connection
 */
esp_err_t eth_connect(void);

/**
 * Counterpart to example_connect, de-initializes Wi-Fi or Ethernet
 */
esp_err_t eth_disconnect(void);

/**
 * @brief Configure stdin and stdout to use blocking I/O
 *
 * This helper function is used in ASIO examples. It wraps installing the
 * UART driver and configuring VFS layer to use UART driver for console I/O.
 */
esp_err_t example_configure_stdin_stdout(void);

/**
 * @brief Returns esp-netif pointer created by example_connect()
 *
 * @note If multiple interfaces active at once, this API return NULL
 * In that case the get_example_netif_from_desc() should be used
 * to get esp-netif pointer based on interface description
 */
esp_netif_t *get_example_netif(void);

/**
 * @brief Returns esp-netif pointer created by example_connect() described by
 * the supplied desc field
 *
 * @param desc Textual interface of created network interface, for example "sta"
 * indicate default WiFi station, "eth" default Ethernet interface.
 *
 */
esp_netif_t *get_example_netif_from_desc(const char *desc);

void num_to_str(char* ip_num);

char* netif_get_ip_info_str(char* Dev_ip_str);

void get_dev_ip_info();

void http_cleanup(esp_http_client_handle_t client);

void __attribute__((noreturn)) task_fatal_error(void);

void infinite_loop(void);

void socket_create();

void socket_close();

void module_set(void);

void create_static_send_package(void);

void create_dynamic_send_package(void);

void send_thread(void *pvParameters);

void udp_server_task(void *pvParameters);

void ota_task(void *pvParameter);

void light_task(void *pvParameter);

#ifdef __cplusplus
}
#endif
