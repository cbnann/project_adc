#ifndef BLE_ADC_H_
#define BLE_ADC_H_

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#define ADC_UUID_BASE { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF,0x12, 0x12, 0x00, 0x00, 0x00, 0x00 }  
#define BLE_UUID_ADC_SEND_SERVICE 0x1525 
#define ADC_UUID_SERVICE        0x1523 
#define BLE_ADC_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3)

typedef struct ble_adc_s ble_adc_t;

typedef void (*ble_adc_data_handler_t) (ble_adc_t * p_nus, uint8_t * p_data, uint16_t length);

typedef struct
{
    ble_adc_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_adc_init_t;

typedef enum
{
    BLE_ADC_EVT_NOTIFICATION_ENABLED,                   /**< Heart Rate value notification enabled event. */
    BLE_ADC_EVT_NOTIFICATION_DISABLED                   /**< Heart Rate value notification disabled event. */
} ble_adc_evt_type_t;

typedef struct
{
    ble_adc_evt_type_t evt_type;                        /**< Type of event. */
} ble_adc_evt_t;

struct ble_adc_s
{
	  
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the S110 SoftDevice). */
   // ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the S110 SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the S110 SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    ble_adc_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};

uint32_t ble_adc_init(ble_adc_t * p_adc, const ble_adc_init_t * p_adc_init);//

void ble_adc_on_ble_evt(ble_adc_t * p_adc, ble_evt_t * p_ble_evt);

uint32_t ble_lbs_on_adc_send(ble_adc_t * p_adc, uint8_t adc_sample);


#endif
