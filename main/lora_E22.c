#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "include/lora_E22.h"

static const int RX_BUF_SIZE = 1024;
#define TXD_PIN (GPIO_NUM_5)
#define RXD_PIN (GPIO_NUM_4)


typedef struct 
{
    uint8_t cmd;
    uint8_t start_addr;
    uint8_t len;
    uint8_t  model;
    uint8_t version;
    uint8_t features;

}module_infos_t;


typedef struct 
{
    uint8_t air_data : 3;
    uint8_t parity_bit :2;
    uint8_t port_rate: 3;
}speed_t;

typedef struct 
{
    uint8_t trans_power : 2;
    uint8_t RSSI_ambient_noise :1;
    uint8_t sub_packet: 2;
    
}option_t;

typedef struct 
{
    uint8_t WOR_period:3;
    uint8_t reserved_2:1;
    uint8_t enable_bt :1;
    uint8_t reserved_1:1;
    uint8_t fixed_trans: 1;
    uint8_t enable_RSSI:1;

}transmision_mode_t;

typedef struct  
{
	uint8_t CRYPT_H;
	uint8_t CRYPT_L;
}crypt_t;

typedef struct 
{
    uint8_t cmd;
    uint8_t start_addr;
    uint8_t len;

    uint8_t addh;
    uint8_t addl;
    uint8_t netid;
    speed_t sp;
    option_t op;
    uint8_t channel;
    transmision_mode_t trans;
    crypt_t crypt;

} configuration_t;

void get_air_data (uint8_t air_d)
{
    char aux[20] = {0};

    switch (air_d)
    {
        case RATE_300:
            strcpy(aux ,"0.3k");    
        break;
        
        case RATE_1200:
            strcpy(aux ,"1.2k ");    
        break;
        
        case RATE_2400:
            strcpy(aux ,"1.2k ");  
        break;
        
        case RATE_4800:
            strcpy(aux ,"4.8k ");    
        break;
        
        case RATE_9600:
            strcpy(aux , "9.6k ");    
        break;

        case RATE_19200:
            strcpy(aux , "19.2k ");
        break;
        case RATE_38400:
            strcpy(aux ,"38.4k ");    
        break;
        
        case RATE_62500:
            strcpy(aux , "62,5k");
        break;

        default:
            strcpy(aux, "NO FIND ");
        break;
    }
   
   printf("AirDate: %s \n",aux);
}

void get_parity_bit (uint8_t par_bit)
{
    char aux[10] = {0};

    switch (par_bit)
    {
        case PARITY_8N1:
            strcpy(aux ,"8N1");    
            break;
        case PARITY_8ON1:
            strcpy(aux ,"8O1");    
            break;
        case PARITY_8E1:
            strcpy(aux ,"8E1");  
            break;
        case PARITY_8N1_EQUAL:
            strcpy(aux , "8N1"); 
            break;  
        default:
            strcpy(aux , "NO FIND");
            break;
    }
    printf("ParityBit: %s\n", aux);
}

void get_baud_rate(uint8_t b_rate)
{
    char aux[10] = {0};
    switch (b_rate)
    {
    case UART_RATE_1200:
        strcpy(aux ,"1200");    
    break;
    
    case UART_RATE_2400:
        strcpy(aux ,"2400");    
    break;

    case UART_RATE_4800:
        strcpy(aux ,"4800");    
    break;
    
    case UART_RATE_9600:
        strcpy(aux ,"9600");    
    break;

    case UART_RATE_19200:
        strcpy(aux ,"19200");    
    break;

    case UART_RATE_38400:
        strcpy(aux ,"38400");    
    break;

    case UART_RATE_57600:
        strcpy(aux ,"57600");    
    break;

    case UART_RATE_115200:
        strcpy(aux ,"115200");    
    break;
    
    default:
        strcpy(aux,"NO FIND");
    break;
    }
    printf("Baudrate : %s\n",aux);
}

void get_sub_packet(uint8_t packet)
{
    char aux[10] = {0};
    switch (packet)
    {
        case SPS_240_00:
         strcpy(aux ,"240 bytes"); 
        break;
        
        case SPS_128_01:
        strcpy(aux ,"128 bytes"); 
        break;
        
        case SPS_064_10:
        strcpy(aux ,"64 bytes");
        break;

        case SPS_032_11:
        strcpy(aux ,"32 bytes");
        break;
    }
    printf("SUB PACKET: %s\n",aux); 
}

void get_wor(uint8_t wor)
{
    char aux[10] = {0};
    switch (wor)
    {
        case WOR_500_000:
         strcpy(aux ,"500ms"); 
        break;
        
        case WOR_1000_001:
        strcpy(aux ,"1000ms"); 
        break;
        
        case WOR_1500_010:
        strcpy(aux ,"1500ms");
        break;

        case WOR_2000_011:
        strcpy(aux ,"2000ms");
        break;
        
        case WOR_2500_100:
        strcpy(aux ,"2500ms");
        break;
        
        case WOR_3000_101:
        strcpy(aux ,"3000ms");
        break;
    
        case WOR_3500_110:
        strcpy(aux ,"3500ms");
        break;

        case WOR_4000_111:
        strcpy(aux ,"4000ms");
        break;
    }
    printf("WOR Cycle: %s\n",aux); 
}

void get_rssi(uint rssi)
{
    char aux[25] = {0};

    switch (rssi)
    {
    case RSSI_ENABLED:
    strcpy(aux ,"ON"); 
    break;

    case RSSI_DISABLED:
       strcpy(aux ,"OFF");
    break;
    
    default:
     strcpy(aux ," ");
    break;
    }
    printf("RSSI STATUS > %s\n",aux);
}

void get_transission(uint8_t trans)
{
    char aux[25] = {0};
    switch (trans)
    {
        case POWER_30:
        strcpy(aux, "30dbm");
        break;

        case POWER_27:
        strcpy(aux, "27dbm");
        break;

        case POWER_24:
        strcpy(aux, "24dbm");
        break;

        case POWER_21:
        strcpy(aux, "21dbm");
        break;
        
        default:
        strcpy(aux, "NO FIND");
        break;
    }

    printf("Power: %s\n",aux);
}

void get_fixe_transission(uint8_t fix)
{

}
gpio_num_t M0 = 22;
gpio_num_t M1 = 23;
gpio_num_t AUX = 24;

void init_uart_lora (void)
{
    const uart_config_t uart_config = 
    {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 8, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); 


    // gpio_set_direction(M0,GPIO_MODE_OUTPUT);
    // gpio_set_direction(M1,GPIO_MODE_OUTPUT);
    // gpio_set_direction(AUX,GPIO_MODE_OUTPUT);
}

esp_err_t write_cmd(uint8_t cmd, uint8_t addr, uint8_t pack_len)
{
    uint8_t CMD[3] = {cmd,addr,pack_len};
    size_t sz = sizeof(CMD);
    if(uart_write_bytes(UART_NUM_1, CMD, sz)<0)
    {
        ESP_LOGI("DEBUG","Falha ao enviar comando");
        return ESP_FAIL;
    }
    ESP_LOG_BUFFER_HEXDUMP("DEBUG", CMD, sz, ESP_LOG_WARN);
    ESP_LOGI("DEBUG","Comando escrito com sucesso | size : %d", sz);
    vTaskDelay(pdMS_TO_TICKS(100));
    return ESP_OK;
}

void set_mode(int mode)
{
   switch (mode)
   {
    case DEEP_SLEEP:
        printf("Configuration - Deep Sleep\n");
        gpio_set_level(M0,1);
        gpio_set_level(M1,1);
    break;
    
    case WOR:
        printf("Configuration - WOR\n");
        gpio_set_level(M0,1);
        gpio_set_level(M1,0);
    break;
    
    case CONFIG:
        printf("Configuration - Mode\n");
        gpio_set_level(M0,0);
        gpio_set_level(M1,1);
    break;
    
    case NORMAL:
        printf("Configuration - Normal\n");
        gpio_set_level(M0,0);
        gpio_set_level(M1,0);
    break;
    
    default:
        break;
   }
   
}


esp_err_t get_information_E22 ()
{
    //set_mode(CONFIG);

    if(write_cmd(READ_CONFIGURATION,REG_ADDRESS_CFG,PL_CONFIGURATION) != ESP_OK)
    {
        return ESP_FAIL;
    }

    size_t sz = sizeof(configuration_t);
    uint8_t* buffer = malloc(sz);
    
    if(buffer ==NULL)
    {
        ESP_LOGI("DEBUG", "Falha na alocação ...");
        return ESP_FAIL;
    }

    if(uart_read_bytes(UART_NUM_1,buffer,sz,pdMS_TO_TICKS(1000)) <= 0)
    {
        ESP_LOGI("DEBUG", "Falha ao pegar as informações");
        free(buffer);
        return  ESP_FAIL;
    }
    ESP_LOG_BUFFER_HEXDUMP("DEBUG", buffer, sz, ESP_LOG_ERROR);
    configuration_t* configs = (configuration_t*)buffer;

    printf("*** *** *** Configs *** *** *** \n");
    printf("HEAD   : %02x ",configs->cmd);  printf(" | %02x",configs->start_addr); printf(" |%02x\n",configs->len);
    printf("Addh   : %02x\n",configs->addh);
    printf("Addl   : %02x\n",configs->addl);
    printf("Channel: %02x\n", configs->channel);

    get_parity_bit(configs->sp.parity_bit);
	get_baud_rate(configs->sp.port_rate);
    get_air_data(configs->sp.air_data);
    
    get_sub_packet(configs->op.sub_packet);
    get_rssi(configs->trans.enable_RSSI);
    get_transission(configs->op.trans_power);
    get_wor(configs->trans.WOR_period);
    printf("*** *** *** End Configs *** *** *** \n");
    
    if(buffer != NULL)
    {
        free(buffer);
    }
    return ESP_OK;

}