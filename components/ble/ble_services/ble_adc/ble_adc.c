#include "ble_adc.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "trace_uart.h"

//#define BLE_UUID_ADC_TX_CHARACTERISTIC 0x0002 

#define BLE_ADC_MAX_TX_CHAR_LEN        BLE_ADC_MAX_DATA_LEN        /**< Maximum length of the TX Characteristic (in bytes). */

//#define ADC_BASE_UUID                  {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */

static void on_connect(ble_adc_t * p_adc, ble_evt_t * p_ble_evt)
{
    p_adc->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_nus     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_adc_t * p_adc, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_adc->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void on_hrm_cccd_write(ble_adc_t * p_adc, ble_gatts_evt_write_t * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        // CCCD written, update notification state
        if (p_adc->data_handler != NULL)
        {
            ble_adc_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_ADC_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_ADC_EVT_NOTIFICATION_DISABLED;
            }

           p_adc->data_handler(p_adc,p_evt_write->data,p_evt_write->len);
        }
    }
}

static void on_write(ble_adc_t * p_adc, ble_evt_t * p_ble_evt)
{
		 ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
//     if (  (p_evt_write->handle == p_adc->tx_handles.value_handle)
//            && (p_evt_write->len == 1) && (p_adc->data_handler != NULL)  )
//		 {
//			 p_adc->data_handler(p_adc,p_evt_write->data,p_evt_write->len);//接收到手机发来的数据后调用
//		 }
		 if (p_evt_write->handle == p_adc->tx_handles.cccd_handle)
    {
        on_hrm_cccd_write(p_adc, p_evt_write);
    }
		 
}

void ble_adc_on_ble_evt(ble_adc_t * p_adc, ble_evt_t * p_ble_evt)
{
	    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_adc, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_adc, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_adc, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static uint32_t ad_send_char_add(ble_adc_t * p_adc, const ble_adc_init_t * p_adc_init)
{
		ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
	
	  memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
		char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
		
		ble_uuid.type = p_adc->uuid_type;
    ble_uuid.uuid = BLE_UUID_ADC_SEND_SERVICE;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
		
		memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_adc->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_adc->tx_handles);
}

uint32_t ble_adc_init(ble_adc_t * p_adc, const ble_adc_init_t * p_adc_init)
{
	  uint32_t   err_code;
    ble_uuid_t ble_uuid;
	  p_adc->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_adc->data_handler = p_adc_init->data_handler;
  	p_adc->is_notification_enabled = false;
	  
	  ble_uuid128_t base_uuid = {ADC_UUID_BASE}; 
	  err_code = sd_ble_uuid_vs_add(&base_uuid, &p_adc->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	
		
		ble_uuid.type = p_adc->uuid_type;
		trace_log("\r\nble_uuid.type=%d\r\n",ble_uuid.type);
    ble_uuid.uuid = ADC_UUID_SERVICE;
		
		
		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_adc->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		
		err_code = ad_send_char_add(p_adc, p_adc_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}

uint32_t ble_lbs_on_adc_send(ble_adc_t * p_adc, uint8_t adc_sample)
{
	  
		ble_gatts_hvx_params_t hvx_params;
	  uint16_t len = sizeof(adc_sample);
		memset(&hvx_params, 0, sizeof(hvx_params));
	  hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
	  hvx_params.handle = p_adc->tx_handles.value_handle;
    hvx_params.p_len  = &len;
    hvx_params.p_data = &adc_sample;
	  return sd_ble_gatts_hvx(p_adc->conn_handle,&hvx_params);
	
}












