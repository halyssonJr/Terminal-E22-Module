#include <stdio.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "include/lora_E22.h"
#include "include/cmd_system.h"


const char* prompt = LOG_COLOR_I "lora> " LOG_RESET_COLOR;
static void init_console(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
    setvbuf(stdin, NULL, _IONBF, 0);
    esp_vfs_dev_uart_port_set_rx_line_endings(0, ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_port_set_tx_line_endings(0 , ESP_LINE_ENDINGS_CRLF);

    const uart_config_t uart_config = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_REF_TICK,

    };
    ESP_ERROR_CHECK( uart_driver_install(0, 256, 0, 0, NULL, 0) );
    ESP_ERROR_CHECK( uart_param_config(0, &uart_config) );
    esp_vfs_dev_uart_use_driver(0);

    esp_console_config_t console_config = 
    {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256,
        .hint_color = atoi(LOG_COLOR_CYAN)
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );
    linenoiseSetMultiLine(1);
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);
    linenoiseHistorySetMaxLen(100);
    linenoiseAllowEmpty(true);
}

static void init_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    
    init_nvs(); 
    init_console();
    esp_console_register_help_command();
    init_uart_lora();
    register_test();

    while(1) 
    {
        
        char* line = linenoise(prompt);
        if (strlen(line) <= 0 ) 
        { 
            // break;
            //Tratar comandos vazios...
        }
     
        else if (strlen(line) > 0) 
        {
            
            linenoiseHistoryAdd(line);
        }

        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) 
        {
            printf("Comando não cadastrado\n");
        } 
        
        else if (err == ESP_ERR_INVALID_ARG) 
        {
            
        } 
        
        else if (err == ESP_OK && ret != ESP_OK) 
        {
            printf("Não retornou um zero: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } 
        
        else if (err != ESP_OK) 
        {
            printf("Erro Interno: %s\n", esp_err_to_name(err));
        }
        
        linenoiseFree(line);
        vTaskDelay(1);
    }

    esp_console_deinit();

}
