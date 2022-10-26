/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include <string.h>
#include "ethernet.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
#include "esp_eth.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif // CONFIG_ETH_USE_SPI_ETHERNET
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
#include "esp_log.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
#define MAX_IP6_ADDRS_PER_NETIF (5)
#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (s_active_interfaces*2)

#if defined(CONFIG_EXAMPLE_CONNECT_IPV6_PREF_LOCAL_LINK)
#define EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE ESP_IP6_ADDR_IS_LINK_LOCAL
#elif defined(CONFIG_EXAMPLE_CONNECT_IPV6_PREF_GLOBAL)
#define EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE ESP_IP6_ADDR_IS_GLOBAL
#elif defined(CONFIG_EXAMPLE_CONNECT_IPV6_PREF_SITE_LOCAL)
#define EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE ESP_IP6_ADDR_IS_SITE_LOCAL
#elif defined(CONFIG_EXAMPLE_CONNECT_IPV6_PREF_UNIQUE_LOCAL)
#define EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE ESP_IP6_ADDR_IS_UNIQUE_LOCAL
#endif // if-elif CONFIG_EXAMPLE_CONNECT_IPV6_PREF_...

#else
#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (s_active_interfaces)
#endif

#define EXAMPLE_DO_CONNECT CONFIG_EXAMPLE_CONNECT_WIFI || CONFIG_EXAMPLE_CONNECT_ETHERNET

#if CONFIG_EXAMPLE_WIFI_SCAN_METHOD_FAST
#define EXAMPLE_WIFI_SCAN_METHOD WIFI_FAST_SCAN
#elif CONFIG_EXAMPLE_WIFI_SCAN_METHOD_ALL_CHANNEL
#define EXAMPLE_WIFI_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#endif

#if CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SIGNAL
#define EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SECURITY
#define EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SECURITY
#endif

#if CONFIG_EXAMPLE_WIFI_AUTH_OPEN
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_EXAMPLE_WIFI_AUTH_WEP
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA_WPA2_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_ENTERPRISE
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_ENTERPRISE
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA3_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_WPA3_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WAPI_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif



static int s_active_interfaces = 0;
static xSemaphoreHandle s_semph_get_ip_addrs;
static esp_netif_t *s_example_esp_netif = NULL;



#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
static esp_ip6_addr_t s_ipv6_addr;

/* types of ipv6 addresses to be displayed on ipv6 events */
static const char *s_ipv6_addr_types[] = {
    "ESP_IP6_ADDR_IS_UNKNOWN",
    "ESP_IP6_ADDR_IS_GLOBAL",
    "ESP_IP6_ADDR_IS_LINK_LOCAL",
    "ESP_IP6_ADDR_IS_SITE_LOCAL",
    "ESP_IP6_ADDR_IS_UNIQUE_LOCAL",
    "ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6"
};
#endif

static const char *TAG = "eth_connect";

#if CONFIG_EXAMPLE_CONNECT_WIFI
static esp_netif_t *wifi_start(void);
static void wifi_stop(void);
#endif
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
static esp_netif_t *eth_start(void);
static void eth_stop(void);
#endif

/**
 * @brief Checks the netif description if it contains specified prefix.
 * All netifs created withing common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
 */
static bool is_our_netif(const char *prefix, esp_netif_t *netif)
{
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}

/* set up connection, Wi-Fi and/or Ethernet */
static void start(void)
{
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    s_example_esp_netif = eth_start();
    s_active_interfaces++;
#endif

#if EXAMPLE_DO_CONNECT
    /* create semaphore if at least one interface is active */
    //s_semph_get_ip_addrs = xSemaphoreCreateCounting(NR_OF_IP_ADDRESSES_TO_WAIT_FOR, 0);
#endif

}

/* tear down connection, release resources */
static void stop(void)
{
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    eth_stop();
    s_active_interfaces--;
#endif
}

#if EXAMPLE_DO_CONNECT
static esp_ip4_addr_t s_ip_addr;

static void on_got_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    if (!is_our_netif(TAG, event->esp_netif)) {
        ESP_LOGW(TAG, "Got IPv4 from another interface \"%s\": ignored", esp_netif_get_desc(event->esp_netif));
        return;
    }
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
    //xSemaphoreGive(s_semph_get_ip_addrs);
}
#endif

#ifdef CONFIG_EXAMPLE_CONNECT_IPV6

static void on_got_ipv6(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    if (!is_our_netif(TAG, event->esp_netif)) {
        ESP_LOGW(TAG, "Got IPv6 from another netif: ignored");
        return;
    }
    esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
    ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif),
             IPV62STR(event->ip6_info.ip), s_ipv6_addr_types[ipv6_type]);
    if (ipv6_type == EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE) {
        memcpy(&s_ipv6_addr, &event->ip6_info.ip, sizeof(s_ipv6_addr));
        //xSemaphoreGive(s_semph_get_ip_addrs);
    }
}

#endif // CONFIG_EXAMPLE_CONNECT_IPV6

esp_err_t eth_connect(void)
{
    /*
    if (s_semph_get_ip_addrs != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    */
    start();
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&stop));
    ESP_LOGI(TAG, "Waiting for IP(s)");
    //for (int i = 0; i < NR_OF_IP_ADDRESSES_TO_WAIT_FOR; ++i) {
    //    xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);
    //}
    
    // iterate over active interfaces, and print out IPs of "our" netifs
    esp_netif_t *netif = NULL;
    esp_netif_ip_info_t ip;
    esp_netif_ip_info_t ip_info;

    
    for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
        netif = esp_netif_next(netif);
        //esp_efuse_mac_get_custom(&Dev_mac);
        esp_netif_get_mac(netif, Dev_mac);
        IP4_ADDR(&ip_info.ip, 10, Dev_mac[3], Dev_mac[4], Dev_mac[5]);
   	    IP4_ADDR(&ip_info.netmask, 255, 0, 0, 0);
        esp_netif_dhcpc_stop(netif);
        ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
        if (is_our_netif(TAG, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));
            printf("- IPv4 address: %d.%d.%d.%d\n" , IP2STR(&ip.ip));
            ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip.ip));

        }
    }
    return ESP_OK;
}

esp_err_t eth_disconnect(void)
{
    if (s_semph_get_ip_addrs == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    vSemaphoreDelete(s_semph_get_ip_addrs);
    s_semph_get_ip_addrs = NULL;
    stop();
    ESP_ERROR_CHECK(esp_unregister_shutdown_handler(&stop));
    return ESP_OK;
}

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}

#ifdef CONFIG_EXAMPLE_CONNECT_IPV6

static void on_wifi_connect(void *esp_netif, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    esp_netif_create_ip6_linklocal(esp_netif);
}

#endif // CONFIG_EXAMPLE_CONNECT_IPV6

static esp_netif_t *wifi_start(void)
{
    char *desc;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    // Prefix the interface description with the module TAG
    // Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
    asprintf(&desc, "%s: %s", TAG, esp_netif_config.if_desc);
    esp_netif_config.if_desc = desc;
    esp_netif_config.route_prio = 128;
    esp_netif_t *netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
    free(desc);
    esp_wifi_set_default_wifi_sta_handlers();

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect, netif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &on_got_ipv6, NULL));
#endif

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_EXAMPLE_WIFI_SSID,
            .password = CONFIG_EXAMPLE_WIFI_PASSWORD,
            .scan_method = EXAMPLE_WIFI_SCAN_METHOD,
            .sort_method = EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD,
            .threshold.rssi = CONFIG_EXAMPLE_WIFI_SCAN_RSSI_THRESHOLD,
            .threshold.authmode = EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        },
    };
    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_connect();
    return netif;
}

static void wifi_stop(void)
{
    esp_netif_t *wifi_netif = get_example_netif_from_desc("sta");
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));
#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &on_got_ipv6));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect));
#endif
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(wifi_netif));
    esp_netif_destroy(wifi_netif);
    s_example_esp_netif = NULL;
}
#endif // CONFIG_EXAMPLE_CONNECT_WIFI

#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET

#ifdef CONFIG_EXAMPLE_CONNECT_IPV6

/** Event handler for Ethernet events */
static void on_eth_event(void *esp_netif, esp_event_base_t event_base,
                         int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_ERROR_CHECK(esp_netif_create_ip6_linklocal(esp_netif));
        break;
    default:
        break;
    }
}

#endif // CONFIG_EXAMPLE_CONNECT_IPV6

static esp_eth_handle_t s_eth_handle = NULL;
static esp_eth_mac_t *s_mac = NULL;
static esp_eth_phy_t *s_phy = NULL;
static esp_eth_netif_glue_handle_t s_eth_glue = NULL;

static esp_netif_t *eth_start(void)
{
    char *desc;
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    // Prefix the interface description with the module TAG
    // Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
    asprintf(&desc, "%s: %s", TAG, esp_netif_config.if_desc);
    esp_netif_config.if_desc = desc;
    esp_netif_config.route_prio = 64;
    esp_netif_config_t netif_config = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    esp_netif_t *netif = esp_netif_new(&netif_config);
    assert(netif);
    free(desc);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    mac_config.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    mac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;

    s_mac = esp_eth_mac_new_esp32(&mac_config);
    s_phy = esp_eth_phy_new_ip101(&phy_config);


    // Install Ethernet driver
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(s_mac, s_phy);
    config.check_link_period_ms = 0xFFFFFFFF;
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &s_eth_handle));

    // combine driver with netif
    s_eth_glue = esp_eth_new_netif_glue(s_eth_handle);
    esp_netif_attach(netif, s_eth_glue);

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &on_got_ip, NULL));

    esp_eth_start(s_eth_handle);
    return netif;
}

static void eth_stop(void)
{
    esp_netif_t *eth_netif = get_example_netif_from_desc("eth");
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &on_got_ip));
#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &on_got_ipv6));
    ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &on_eth_event));
#endif
    ESP_ERROR_CHECK(esp_eth_stop(s_eth_handle));
    ESP_ERROR_CHECK(esp_eth_del_netif_glue(s_eth_glue));
    ESP_ERROR_CHECK(esp_eth_driver_uninstall(s_eth_handle));
    ESP_ERROR_CHECK(s_phy->del(s_phy));
    ESP_ERROR_CHECK(s_mac->del(s_mac));

    esp_netif_destroy(eth_netif);
    s_example_esp_netif = NULL;
}

#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

esp_netif_t *get_example_netif(void)
{
    return s_example_esp_netif;
}

esp_netif_t *get_example_netif_from_desc(const char *desc)
{
    esp_netif_t *netif = NULL;
    char *expected_desc;
    asprintf(&expected_desc, "%s: %s", TAG, desc);
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (strcmp(esp_netif_get_desc(netif), expected_desc) == 0) {
            free(expected_desc);
            return netif;
        }
    }
    free(expected_desc);
    return netif;
}

void num_to_str(char* ip_num)   //put 4 separated ip nums into a string
{
    char ip_str1[16];
    char ip_str2[16];
    char ip_str3[16];
    char ip_str_dot[16] = {'.'};
    //ESP_LOGI(TAG, "ip_num:%d.%d.%d.%d", ip_num[0],ip_num[1],ip_num[2],ip_num[3]);
    itoa(ip_num[0],Dest_ip_str,10);
    itoa(ip_num[1],ip_str1,10);
    itoa(ip_num[2],ip_str2,10);
    itoa(ip_num[3],ip_str3,10);
    strcat(Dest_ip_str,ip_str_dot);
    strcat(Dest_ip_str,ip_str1);
    strcat(Dest_ip_str,ip_str_dot);
    strcat(Dest_ip_str,ip_str2);
    strcat(Dest_ip_str,ip_str_dot);
    strcat(Dest_ip_str,ip_str3);
    //ESP_LOGI(TAG, "Dest_ip_str:%s", Dest_ip_str);
}

char* netif_get_ip_info_str(char* Dev_ip_str)   //put 4 separated ip nums into a string
{
    uint8_t Dev_IPV4[4];
    char Dev_ip_str1[16];
    char Dev_ip_str2[16];
    char Dev_ip_str3[16];
    char Dev_ip_str_dot[16] = {'.'};
    Dev_IPV4[0] = 0x0A;
    for(int i=1;i<4;i++)Dev_IPV4[i]=Dev_mac[i+2];
    //ESP_LOGI(TAG, "Device IP:%d.%d.%d.%d", Dev_IPV4[0],Dev_IPV4[1],Dev_IPV4[2],Dev_IPV4[3]);
    itoa(Dev_IPV4[0],Dev_ip_str,10);
    itoa(Dev_IPV4[1],Dev_ip_str1,10);
    itoa(Dev_IPV4[2],Dev_ip_str2,10);
    itoa(Dev_IPV4[3],Dev_ip_str3,10);
    strcat(Dev_ip_str,Dev_ip_str_dot);
    strcat(Dev_ip_str,Dev_ip_str1);
    strcat(Dev_ip_str,Dev_ip_str_dot);
    strcat(Dev_ip_str,Dev_ip_str2);
    strcat(Dev_ip_str,Dev_ip_str_dot);
    strcat(Dev_ip_str,Dev_ip_str3);

    return Dev_ip_str;
}

void get_dev_ip_info()
{
    Dev_ip_num[0] = 0x0A;
    for(int i=1;i<4;i++) Dev_ip_num[i]=Dev_mac[i+2];
    //ESP_LOGI(TAG, "Dev_ip_num = %d.%d.%d.%d", Dev_ip_num[0],Dev_ip_num[1],Dev_ip_num[2],Dev_ip_num[3]);
}

void http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

void __attribute__((noreturn)) task_fatal_error(void)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");
    (void)vTaskDelete(NULL);

    while (1) {
        ;
    }
}

void infinite_loop(void)
{
    int i = 0;
    ESP_LOGI(TAG, "When a new firmware is available on the server, press the reset button to download it");
    while(1) {
        ESP_LOGI(TAG, "Waiting for a new firmware ... %d", ++i);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void socket_create()
{
    sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        while(1);
    }
    else ESP_LOGE(TAG, "Socket created");
}

void socket_close()
{
    ESP_LOGE(TAG, "Shutting down socket...");
    shutdown(sock, 0);
    close(sock);
    shutdown(socket_fd, 0);
    close(socket_fd);
}

void module_set(void)
{
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Write
        printf("Updating restart counter in NVS ... ");
        // Module_Type = Normal_Module;
        err = nvs_set_u8(my_handle, "Module_Type", Module_Type);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }
    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
}

void create_static_send_package(void)
{
    esp_app_desc_t *app = esp_ota_get_app_description(); // Get firmware version
    int version = atoi(app->version);
    // UDP send package contents (static contents)
    EtherIAMHERE_send.Header.Type = UVM_Message_Type_IAMHERE;
    for (int i = 0; i < 4; i++)
    {
        EtherIAMHERE_send.Header.SenderIp[i] = Dev_ip_num[i];
        EtherIAMHERE_send.Header.ReciverIp[i] = Dest_ip_num[i];
    }

    EtherIAMHERE_send.versionH = version / 100;
    EtherIAMHERE_send.versionL = version % 100;
    EtherIAMHERE_send.module_type = Module_Type;
    EtherIAMHERE_send.UVLightSource2 = esp_reset_reason();
    for (int i = 0; i < 8; i++)
        EtherIAMHERE_send.Device_Info[i] = 0;

    if (UDP_Send_Type == UVM_Message_Type_EtherFirmwareUpdate)
    {
        get_dev_ip_info();
        Firmware_Update_send.Header.Type = UVM_Message_Type_EtherFirmwareUpdate;
        for (int i = 0; i < 4; i++)
        {
            Firmware_Update_send.Header.SenderIp[i] = Dev_ip_num[i];
            Firmware_Update_send.Header.ReciverIp[i] = Dest_ip_num[i];
        }
        strcpy(Firmware_Update_send.UpdateText, "UPDATE");

        Firmware_Update_send.versionH = version / 100;
        Firmware_Update_send.versionL = version % 100;
        strcpy(Firmware_Update_send.Path, "http://10.187.1.2:8070/UV_V24_Demo.bin");
    }

    else if (UDP_Send_Type == UVM_Message_Type_CONTROL_UV)
    {
        EtherControlUV_send.Header.Type = UVM_Message_Type_CONTROL_UV;
        for (int i = 0; i < 4; i++)
        {
            EtherControlUV_send.Header.SenderIp[i] = Dev_ip_num[i];
            EtherControlUV_send.Header.ReciverIp[i] = Dest_ip_num[i];
        }
    }
    else if (UDP_Send_Type == UVM_Message_Type_Set_Type)
    {
        EtherSetType_send.Header.Type = UVM_Message_Type_Set_Type;
        for (int i = 0; i < 4; i++)
        {
            EtherSetType_send.Header.SenderIp[i] = Dev_ip_num[i];
            EtherSetType_send.Header.ReciverIp[i] = Dest_ip_num[i];
            EtherSetType_send.module_type = Lidar_Stop_Module;
        }
    }
    else if (UDP_Send_Type == UVM_Message_Type_Set_RGB)
    {
        EtherSetRGB_send.Header.Type = UVM_Message_Type_Set_RGB;
        for (int i = 0; i < 4; i++)
        {
            EtherSetRGB_send.Header.SenderIp[i] = Dev_ip_num[i];
            EtherSetRGB_send.Header.ReciverIp[i] = Dest_ip_num[i];
            EtherSetRGB_send.light_color = Red;
        }
    }
    else if (UDP_Send_Type == UVM_Message_Type_Reboot)
    {
        EtherReboot_send.Header.Type = UVM_Message_Type_Reboot;
        for (int i = 0; i < 4; i++)
        {
            EtherReboot_send.Header.SenderIp[i] = Dev_ip_num[i];
            EtherReboot_send.Header.ReciverIp[i] = Dest_ip_num[i];
            EtherReboot_send.reboot = 1;
        }
    }
    //if (Module_Type == Display_Module)
    if (module_info[Module_Type].HasDisplay == true)
    {
        EtherTraffic_send.Header.Type = UVM_Message_Type_Traffic;
        for (int i = 0; i < 4; i++)
        {
            EtherTraffic_send.Header.SenderIp[i] = Dev_ip_num[i];
            EtherTraffic_send.Header.ReciverIp[i] = Dest_ip_num[i];
        }
    }
}
int rpmFake = 0;
void create_dynamic_send_package(void)
{
    // UDP send package contents (dynamic contents and sendto function)
    EtherIAMHERE_send.LidarLength = distance;
    EtherIAMHERE_send.Temperature = temp;
    EtherIAMHERE_send.FanRPM = rpmFake++;
    EtherIAMHERE_send.UVIntensity = uv_intensity;
    EtherIAMHERE_send.UVLightSource1 = UVLightSource1_status;
    
    EtherIAMHERE_send.HallVoltage = hall_sensor_voltage;
    EtherIAMHERE_send.Module_on_time = Module_time_log;
    EtherIAMHERE_send.UV_on_time = UV_time_log;
    EtherTraffic_send.LCD_page = LCD_Page;

    int t = uvMiliLeft;
    if (t > 200)
    {
        t = 200;
    }
    if (LCD_Page == Display_Hold && stop_sensor_flag != true && UV_On_UDP_Flag == true)
    {
        EtherControlUV_send.OnTimeMiliSec = t;
        //EtherIAMHERE_send.UVLightSource1 = 0;
    }
    else
    {
        EtherControlUV_send.OnTimeMiliSec = 0;
        //EtherIAMHERE_send.UVLightSource1 = 1;
    }

    //if (Module_Type != Display_Module)
    //{
        for (int i = 0; i < 8; i++)
        {
            EtherIAMHERE_send.Device_Info[i] = Device_missing[i];
        }
        /*
        //if(Module_Type == Check_Module)
        if (module_info[Module_Type].HasCheckTask == true)
        {
            if(EtherIAMHERE_send.Device_Info[0] >= 1)
            {
                printf("Missing number = %d %d %d %d %d %d %d %d\n",\
                 EtherIAMHERE_send.Device_Info[0],EtherIAMHERE_send.Device_Info[1],EtherIAMHERE_send.Device_Info[2],\
                 EtherIAMHERE_send.Device_Info[3],EtherIAMHERE_send.Device_Info[4],EtherIAMHERE_send.Device_Info[5],\
                 EtherIAMHERE_send.Device_Info[6],EtherIAMHERE_send.Device_Info[7]);
            }
        }*/
    //}
    // EtherIAMHERE_send.Device_Info[7] =1;
}

void udp_server_task(void *pvParameters)
{
    ESP_LOGI(TAG, "UDP server task started.");
    uint8_t UV_check_cnt = 0;
    bool UV_check_flag = false;
    bool Dev_in_library = false;
    int flag_on = 1;
    struct sockaddr_in multicast_addr;
    unsigned short multicast_port;
    /*
    if (Module_Type == Display_Module || Module_Type == Check_Module)
        multicast_port = atoi("55601");
    else
        multicast_port = atoi("55603");*/
    multicast_port = module_info[Module_Type].receive_port;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        ESP_LOGE(TAG, "socket() failed");
        exit(1);
    }

    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag_on, sizeof(flag_on))) < 0)
    {
        ESP_LOGE(TAG, "setsockopt() failed");
        exit(1);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicast_addr.sin_port = htons(multicast_port);

    if ((bind(sock, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr))) < 0)
    {
        ESP_LOGE(TAG, "bind() failed");
        exit(1);
    }

    while (1)
    {
        // ESP_LOGI(TAG, "Start of receiving package.");
        // ESP_LOGI(TAG, "Waiting for data");
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

        if (len < 0)        // Error occurred during receiving
        {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }
        else                // Data received
        {
            // Get the sender's ip address as string
            // inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
            // ESP_LOGI(TAG, "Received %d bytes from %s", len, addr_str);
            // EtherHeader* header = (EtherHeader*)rx_buffer;
            udp_rec_type = rx_buffer[0];

            if (udp_rec_type == UVM_Message_Type_EtherFirmwareUpdate && firmware_updating_flag == false)
            {
                // EtherFirmwareUpdate* Firmware_Update_rec = (EtherFirmwareUpdate*)rx_buffer;
                memcpy(&Firmware_Update_rec, rx_buffer, sizeof(Firmware_Update_rec));
                if (memcmp(Firmware_Update_rec.UpdateText, "UPDATE", 6) == 0 &&
                    Firmware_Update_rec.Header.Type == UVM_Message_Type_EtherFirmwareUpdate)
                {
                    ota_start_flag = true;
                    Shutup_Flag = true;
                    ESP_LOGE(TAG, "Delete sockets and udp server task.");
                    socket_close();
                    vTaskDelete(NULL);
                }
                /*
                ESP_LOGI(TAG, "************************");
                ESP_LOGI(TAG, "Type:%d", Firmware_Update_rec.Header.Type);
                ESP_LOGI(TAG, "SenderIp:%d.%d.%d.%d", Firmware_Update_rec.Header.SenderIp[0],Firmware_Update_rec.Header.SenderIp[1],\
                Firmware_Update_rec.Header.SenderIp[2],Firmware_Update_rec.Header.SenderIp[3]);
                ESP_LOGI(TAG, "ReciverIp:%d.%d.%d.%d", Firmware_Update_rec.Header.ReciverIp[0],Firmware_Update_rec.Header.ReciverIp[1],\
                Firmware_Update_rec.Header.ReciverIp[2],Firmware_Update_rec.Header.ReciverIp[3]);
                ESP_LOGI(TAG, "UpdateText:%s", Firmware_Update_rec.UpdateText);
                ESP_LOGI(TAG, "versionH:%d", Firmware_Update_rec.versionH);
                ESP_LOGI(TAG, "versionL:%d", Firmware_Update_rec.versionL);
                ESP_LOGI(TAG, "Path:%s", Firmware_Update_rec.Path);
                ESP_LOGI(TAG, "************************");
                */
            }
            else if (udp_rec_type == UVM_Message_Type_IAMHERE)
            {
                
                EtherIAMHERE *EtherIAMHERE_rec1;
                EtherIAMHERE_rec1 = &rx_buffer;
              //  memcpy(&EtherIAMHERE_rec, rx_buffer, sizeof(EtherIAMHERE_rec));
                //if (Module_Type == Display_Module)
                if (module_info[Module_Type].HasDisplay == true)
                {
                    if (EtherIAMHERE_rec1->module_type == Hall1_Module)
                        Hall1_voltage = EtherIAMHERE_rec1->HallVoltage;
                    else if (EtherIAMHERE_rec1->module_type == Hall2_Module)
                        Hall2_voltage = EtherIAMHERE_rec1->HallVoltage;
                    else if (EtherIAMHERE_rec1->module_type == Lidar_Start_Module)
                        Start_distance = EtherIAMHERE_rec1->LidarLength;
                    else if (EtherIAMHERE_rec1->module_type == Lidar_Stop_Module)
                        Stop_distance = EtherIAMHERE_rec1->LidarLength;
                    else if (EtherIAMHERE_rec1->module_type == Check_Module && module_info[Module_Type].HasCheckTask == false)
                    {
                        if (EtherIAMHERE_rec1->Device_Info[0] >= 1)
                        {
                            Device_missing_flag = true;
                        }
                        else
                            Device_missing_flag = false;
                    }
                }
                //if (Module_Type == Check_Module)
                if (module_info[Module_Type].HasCheckTask == true)
                {    
                    Dev_ip_input = EtherIAMHERE_rec1->Header.SenderIp[0];       // Getting ip addr of the UDP pack
                    Dev_ip_input |= (EtherIAMHERE_rec1->Header.SenderIp[1] << 8);
                    Dev_ip_input |= (EtherIAMHERE_rec1->Header.SenderIp[2] << 16);
                    Dev_ip_input |= (EtherIAMHERE_rec1->Header.SenderIp[3] << 24);

                    Dev_in_library = false;
                    /**********************************************************************
                     * Function of the following loop and if
                     * Check if this ip addr is already in the lib
                     *   if no  --> (1) Store this ip into the lib
                     *              (2) Log total active device number
                     *   if yes --> (1) Log received pack number from this ip
                     *              (2) Check if this module turn on the UV light successfully
                     **********************************************************************/
                    for (int i = 0; i <= Active_dev_num; i++)   
                    { 
                        if (Dev_ip_input == Dev_ip_library[i][0] && Dev_in_library == false)    
                        { 
                            Dev_in_library = true;                                              // Device not in the lib, put it in.
                            Dev_ip_library[i][2]++;
                            Dev_ip_library[i][3] = EtherIAMHERE_rec1->Device_Info[0];
                            if(EtherIAMHERE_rec1->Device_Info[0] != 0){
                                 //printf("Dead light source\n");
                            }
                            break;
                            // printf("i = %d, Active_dev_num = %d\n", i, Active_dev_num);
                        }
                    }

                    if (Dev_in_library == false)    // Device not in the lib, put it in.
                    { 
                        Dev_ip_library[Active_dev_num][0] = Dev_ip_input;                       //ip addr
                        Dev_ip_library[Active_dev_num][1] = EtherIAMHERE_rec1->module_type;     //module type
                        Dev_ip_library[Active_dev_num][2]++;                                    //cnt for checking if this mudule is missing
                        Dev_ip_library[Active_dev_num][3] = EtherIAMHERE_rec1->Device_Info[0];  //0 not missing   1 missing
                        printf("Dev_ip_library[%d][0] = %X \n", Active_dev_num, Dev_ip_library[Active_dev_num][0]);
                        if (EtherIAMHERE_rec1->module_type == Normal_Module)
                            Active_normal_num++;
                        Active_dev_num++;
                    }
                    // Check UV light status
                   /* if (EtherIAMHERE_rec.Device_Info[0] == 1 && Dev_ip_input != Dev_uv_error)
                    {
                        for (int i = 0; i < 8; i++)
                            Device_missing[i] = EtherIAMHERE_rec.Device_Info[i];
                        printf("Missing number from normal module = %d %d %d %d %d %d %d %d\n",\
                        EtherIAMHERE_rec.Device_Info[0],EtherIAMHERE_rec.Device_Info[1],EtherIAMHERE_rec.Device_Info[2],\
                        EtherIAMHERE_rec.Device_Info[3],EtherIAMHERE_rec.Device_Info[4],EtherIAMHERE_rec.Device_Info[5],\
                        EtherIAMHERE_rec.Device_Info[6],EtherIAMHERE_rec.Device_Info[7]);
                        
                        Dev_uv_error = Dev_ip_input;
                    }*/
                    /*
                    if(EtherIAMHERE_rec.module_type == Display_Module){
                        if(UVStatus_check != EtherIAMHERE_rec.UVLightSource1) {         //Check start waiting
                            UV_check_cnt ++;
                            if(UV_check_cnt == 3){
                                UV_check_cnt = 0;
                                UV_check_flag = true;
                                UVStatus_check = EtherIAMHERE_rec.UVLightSource1;
                                printf("UVStatus_check = %d UV_check_cnt = %d\n",UVStatus_check, UV_check_cnt);
                            }
                        }
                        if(UV_check_flag == true) {                                     //Check stop waiting
                            UV_check_cnt_2 ++;
                            if(UV_check_cnt_2 == 3 || UVStatus_check != EtherIAMHERE_rec.UVLightSource1){
                                UV_check_cnt_2 = 0;
                                UV_check_flag = false;
                            }
                        }
                    }
                    if(EtherIAMHERE_rec.UVLightSource1 != UVStatus_check && UV_check_flag == true) {
                        Device_missing[0] = 1;
                        Device_missing[EtherIAMHERE_rec.module_type] = 1;
                    }
                    */
                }
            }
            else if (udp_rec_type == UVM_Message_Type_CONTROL_UV)
            {
                EtherControlUV* EtherControlUV_rec1 = (EtherControlUV*)rx_buffer;
               // memcpy(&EtherControlUV_rec, rx_buffer, sizeof(EtherControlUV_rec));
                uv_on_time_ms = EtherControlUV_rec1->OnTimeMiliSec;
                /*
                ESP_LOGI(TAG, "************************");
                ESP_LOGI(TAG, "Type:%d", EtherControlUV_rec.Header.Type);
                ESP_LOGI(TAG, "SenderIp:%d.%d.%d.%d", EtherControlUV_rec.Header.SenderIp[0],EtherControlUV_rec.Header.SenderIp[1],\
                EtherControlUV_rec.Header.SenderIp[2],EtherControlUV_rec.Header.SenderIp[3]);
                ESP_LOGI(TAG, "ReciverIp:%d.%d.%d.%d", EtherControlUV_rec.Header.ReciverIp[0],EtherControlUV_rec.Header.ReciverIp[1],\
                EtherControlUV_rec.Header.ReciverIp[2],EtherControlUV_rec.Header.ReciverIp[3]);
                ESP_LOGI(TAG, "OnTimeMiliSec:%d", EtherControlUV_rec.OnTimeMiliSec);
                ESP_LOGI(TAG, "************************");
                */
            }
            else if (udp_rec_type == UVM_Message_Type_Set_Type)
            {
                memcpy(&EtherSetType_rec, rx_buffer, sizeof(EtherSetType_rec));
                if (Module_Type != EtherSetType_rec.module_type)
                {
                    Module_Type = EtherSetType_rec.module_type;
                    module_set(); // Set module type here
                    /*
                    ESP_LOGI(TAG, "************************");
                    ESP_LOGI(TAG, "Type:%d", EtherSetType_rec.Header.Type);
                    ESP_LOGI(TAG, "SenderIp:%d.%d.%d.%d", EtherSetType_rec.Header.SenderIp[0], EtherSetType_rec.Header.SenderIp[1],
                             EtherSetType_rec.Header.SenderIp[2], EtherSetType_rec.Header.SenderIp[3]);
                    ESP_LOGI(TAG, "ReciverIp:%d.%d.%d.%d", EtherSetType_rec.Header.ReciverIp[0], EtherSetType_rec.Header.ReciverIp[1],
                             EtherSetType_rec.Header.ReciverIp[2], EtherSetType_rec.Header.ReciverIp[3]);
                    ESP_LOGI(TAG, "module_type:%d", EtherSetType_rec.module_type);
                    ESP_LOGI(TAG, "************************");
                    */
                }
            }
            else if (udp_rec_type == UVM_Message_Type_Set_RGB)
            {
                memcpy(&EtherSetRGB_rec, rx_buffer, sizeof(EtherSetRGB_rec));
                RGB_light_color = EtherSetRGB_rec.light_color;
            }
            else if (udp_rec_type == UVM_Message_Type_Reboot)
            {
                memcpy(&EtherReboot_rec, rx_buffer, sizeof(EtherReboot_rec));
                if (EtherReboot_rec.reboot == 1)
                    esp_restart();
            }
            else if (udp_rec_type == UVM_Message_Type_Traffic)
            {
                memcpy(&EtherTraffic_rec, rx_buffer, sizeof(EtherTraffic_rec));
                LCD_Page = EtherTraffic_rec.LCD_page;
            }
            else if (udp_rec_type == UVM_Message_Type_Shutup)
            {
                memcpy(&EtherShutup_rec, rx_buffer, sizeof(EtherShutup_rec));
                Shutup_Flag = EtherShutup_rec.Shutup;
            }
            // rx_buffer[0] == UVM_Message_Type_CONTROL_UV
            // rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...

            /* Getting SenderIp
            int j = 0;
            int z = 0;
            char num[3] = {0};
            for(int i = 0; i<sizeof(addr_str); i++){
                if((addr_str[i] == '.')||(addr_str[i] == 0)){
                    //ESP_LOGI(TAG,"i=%d", i);
                    j = 0;
                    Firmware_Update_rec.Header.SenderIp[z] = atoi(num);
                    //ESP_LOGI(TAG, "SenderIp[%d] = %d:",z, Firmware_Update_rec.Header.SenderIp[z]);
                    z++;
                    for(int i=0;i<4;i++)num[i] = 0;
                    if(addr_str[i] == 0)break;
                }
                else{
                    num[j] = addr_str[i];
                    //ESP_LOGI(TAG, "num[%d] = %d:",j, num[j]);
                    //ESP_LOGI(TAG, "num = %s:", num);
                    j++;
                }
            }
            */

            // Getting device Ip
            // get_dev_ip_info();

            // for(int i=0;i<4;i++)Firmware_Update_rec.Header.ReciverIp[i]=Dev_ip_num[i];

            // print_package_info(Firmware_Update_rec);

        }
        udp_server_task_cnt ++;
    }

    ESP_LOGI(TAG, "udp_server_task Deleted");
    vTaskDelete(NULL);
}

void send_thread(void *pvParameters)
{
    ESP_LOGI(TAG, "UDP send thread started.");
    struct sockaddr_in info_sock, control_sock;
    int sent_data = 0;
    uint16_t cnt = 0;

    tcpip_adapter_ip_info_t ipInfo;
    char str[256];
    get_dev_ip_info();
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &ipInfo);

    memset(&control_sock, 0, sizeof(struct sockaddr_in));
    control_sock.sin_family = AF_INET;
    control_sock.sin_addr.s_addr = inet_addr(RECEIVER_IP_ADDR);
    control_sock.sin_port = htons(CONTROL_PORT);    //CONTROL_PORT 55603
    inet_pton(AF_INET, RECEIVER_IP_ADDR, &control_sock.sin_addr);

    memset(&info_sock, 0, sizeof(struct sockaddr_in));
    info_sock.sin_family = AF_INET;
    info_sock.sin_addr.s_addr = inet_addr(RECEIVER_IP_ADDR);
    info_sock.sin_port = htons(IAMHERE_PORT);   //IAMHERE_PORT 55601
    inet_pton(AF_INET, RECEIVER_IP_ADDR, &info_sock.sin_addr);

    // Set UDP send type based on module type
    //if (Module_Type == Display_Module)
    if (module_info[Module_Type].HasDisplay == true)
        UDP_Send_Type = UVM_Message_Type_CONTROL_UV;
    else
        UDP_Send_Type = UVM_Message_Type_IAMHERE;
    // printf("Module_Type = %d\n",Module_Type);
    printf("UDP_Send_Type = %d\n", UDP_Send_Type);

    create_static_send_package();
    //timer_start(TIMER_GROUP_0, TIMER_1);
    
    
    while (1)        // UDP send package contents (dynamic contents and sendto function)
    {
        socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0)
            printf("socket call failed");
        cnt++;
        create_dynamic_send_package();
        
        if (UDP_Send_Type == UVM_Message_Type_IAMHERE)
        {
            if (Shutup_Flag == 0 )
                sent_data = sendto(socket_fd, &EtherIAMHERE_send, sizeof(EtherIAMHERE_send), 0, (struct sockaddr *)&info_sock, sizeof(info_sock));
        }
        else if (UDP_Send_Type == UVM_Message_Type_CONTROL_UV)
        {
            if (Shutup_Flag == 0 && cnt % 10 == 0)
                sent_data = sendto(socket_fd, &EtherIAMHERE_send, sizeof(EtherIAMHERE_send), 0, (struct sockaddr *)&info_sock, sizeof(info_sock));
            else if (LCD_Change_Flag == true)
            {
                sent_data = sendto(socket_fd, &EtherTraffic_send, sizeof(EtherTraffic_send), 0, (struct sockaddr *)&control_sock, sizeof(control_sock));
                LCD_Change_Flag = false;
            }
            else //if(UV_On_UDP_Flag == true)
            sent_data = sendto(socket_fd, &EtherControlUV_send, sizeof(EtherControlUV_send), 0, (struct sockaddr *)&control_sock, sizeof(control_sock));
        }
        /*  Modules will not send following commands, application will do.
        else if (UDP_Send_Type == UVM_Message_Type_EtherFirmwareUpdate)
        {
            sent_data = sendto(socket_fd, &Firmware_Update_send, sizeof(Firmware_Update_send), 0, (struct sockaddr *)&control_sock, sizeof(control_sock));
        }
        else if (UDP_Send_Type == UVM_Message_Type_Set_Type)
        {
            sent_data = sendto(socket_fd, &EtherSetType_send, sizeof(EtherSetType_send), 0, (struct sockaddr *)&control_sock, sizeof(control_sock));
        }
        else if (UDP_Send_Type == UVM_Message_Type_Set_RGB)
        {
            sent_data = sendto(socket_fd, &EtherSetRGB_send, sizeof(EtherSetRGB_send), 0, (struct sockaddr *)&control_sock, sizeof(control_sock));
        }
        */
        vTaskDelay(((module_info[Module_Type].UDP_sending_period) / 2) / portTICK_PERIOD_MS); //Half of the UDP sending period
        shutdown(socket_fd, 0);
        close(socket_fd);
        vTaskDelay(((module_info[Module_Type].UDP_sending_period) / 2) / portTICK_PERIOD_MS); //Half of the UDP sending period

        // UDP send package check
        if (sent_data < 0)
        {
            ESP_LOGE(TAG, "Unable to send package: errno %d", errno);
            printf("send failed\n");
            if (ota_start_flag == false && firmware_updating_flag == false && ota_running_flag == false)
            {
                //esp_restart();
            }
        }
        send_thread_task_cnt ++;
        if(watchdog_running == true)CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
    }
    close(socket_fd);
    vTaskDelete(NULL);
}

void ota_task(void *pvParameter)
{
    esp_err_t err;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;

    ESP_LOGI(TAG, "Starting OTA example");
    ota_running_flag = true;

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running)
    {
        ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
                 configured->address, running->address);
        ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)", running->type, running->subtype, running->address);

    esp_http_client_config_t config = {
        .url = Firmware_Update_rec.Path,
        //.cert_pem = (char *)server_cert_pem_start,
        .timeout_ms = CONFIG_EXAMPLE_OTA_RECV_TIMEOUT,
        .keep_alive_enable = true,
    };

#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialise HTTP connection");
        esp_restart();
        task_fatal_error();
    }
    err = esp_http_client_open(client, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_restart();
        esp_http_client_cleanup(client);
        task_fatal_error();
    }
    esp_http_client_fetch_headers(client);

    update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
             update_partition->subtype, update_partition->address);

    int binary_file_length = 0;
    /*deal with all receive packet*/
    bool image_header_was_checked = false;
    while (1)
    {
        int data_read = esp_http_client_read(client, ota_write_data, BUFFSIZE);
        if (data_read < 0)
        {
            ESP_LOGE(TAG, "Error: SSL data read error");
            esp_restart();
            http_cleanup(client);
            task_fatal_error();
        }
        else if (data_read > 0)
        {
            if (image_header_was_checked == false)
            {
                esp_app_desc_t new_app_info;
                if (data_read > sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t))
                {
                    // check current version with downloading
                    memcpy(&new_app_info, &ota_write_data[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)], sizeof(esp_app_desc_t));
                    ESP_LOGI(TAG, "New firmware version: %s", new_app_info.version);

                    esp_app_desc_t running_app_info;
                    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
                    {
                        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
                    }

                    const esp_partition_t *last_invalid_app = esp_ota_get_last_invalid_partition();
                    esp_app_desc_t invalid_app_info;
                    if (esp_ota_get_partition_description(last_invalid_app, &invalid_app_info) == ESP_OK)
                    {
                        ESP_LOGI(TAG, "Last invalid firmware version: %s", invalid_app_info.version);
                    }

                    // check current version with last invalid partition
                    if (last_invalid_app != NULL)
                    {
                        if (memcmp(invalid_app_info.version, new_app_info.version, sizeof(new_app_info.version)) == 0)
                        {
                            ESP_LOGW(TAG, "New version is the same as invalid version.");
                            ESP_LOGW(TAG, "Previously, there was an attempt to launch the firmware with %s version, but it failed.", invalid_app_info.version);
                            ESP_LOGW(TAG, "The firmware has been rolled back to the previous version.");
                            http_cleanup(client);
                            infinite_loop();
                        }
                    }
#ifndef CONFIG_EXAMPLE_SKIP_VERSION_CHECK
                    if (memcmp(new_app_info.version, running_app_info.version, sizeof(new_app_info.version)) == 0)
                    {
                        ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
                        http_cleanup(client);
                        break;
                        // infinite_loop();
                    }
                    else
                        firmware_updating_flag = true;
#endif

                    image_header_was_checked = true;

                    err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
                    if (err != ESP_OK)
                    {
                        ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                        esp_restart();
                        http_cleanup(client);
                        esp_ota_abort(update_handle);
                        task_fatal_error();
                    }
                    ESP_LOGI(TAG, "esp_ota_begin succeeded");
                }
                else
                {
                    ESP_LOGE(TAG, "received package is not fit len");
                    esp_restart();
                    http_cleanup(client);
                    esp_ota_abort(update_handle);
                    task_fatal_error();
                }
            }
            err = esp_ota_write(update_handle, (const void *)ota_write_data, data_read);
            if (err != ESP_OK)
            {
                http_cleanup(client);
                esp_ota_abort(update_handle);
                task_fatal_error();
            }
            binary_file_length += data_read;
            ESP_LOGD(TAG, "Written image length %d", binary_file_length);
        }
        else if (data_read == 0)
        {
            /*
             * As esp_http_client_read never returns negative error code, we rely on
             * `errno` to check for underlying transport connectivity closure if any
             */
            if (errno == ECONNRESET || errno == ENOTCONN)
            {
                ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
                esp_restart();
                break;
            }
            if (esp_http_client_is_complete_data_received(client) == true)
            {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
        }
    }
    if (firmware_updating_flag == true)
    {

        ESP_LOGI(TAG, "Total Write binary data length: %d", binary_file_length);
        if (esp_http_client_is_complete_data_received(client) != true)
        {
            ESP_LOGE(TAG, "Error in receiving complete file");
            esp_restart();
            http_cleanup(client);
            esp_ota_abort(update_handle);
            task_fatal_error();
        }

        err = esp_ota_end(update_handle);
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_OTA_VALIDATE_FAILED)
            {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            else
            {
                ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
            }
            esp_restart();
            http_cleanup(client);
            task_fatal_error();
        }

        err = esp_ota_set_boot_partition(update_partition);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
            esp_restart();
            http_cleanup(client);
            task_fatal_error();
        }
        firmware_updating_flag = false;
        ESP_LOGI(TAG, "Prepare to restart system!");
        esp_restart();
        return;
    }
    ota_end_flag = true;
    vTaskDelete(NULL);
}
