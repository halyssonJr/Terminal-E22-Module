
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include <stdio.h>

#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#include <ctype.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "include/cmd_system.h"
#include "include/lora_E22.h"


static int get_infos_LoRa(int argc, char **argv)
{
    get_information_E22();
    return 0;
}
void register_get_infos()
{
      const esp_console_cmd_t cmd = {
        .command = "get_info",
        .help = "mostra as informações coletadas do E22",
        .hint = NULL,
        .func = &get_infos_LoRa,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
void register_test(void)
{
    
    // Chama todas as funções que serão executad para a giga de testes
    register_get_infos();

    printf("\n"
           "Terminal LoRa E22.\n"
           "Para navegar pelo terminal use as setas para cima e para baixo.\n"
           "Digitando 'help' vai encontra uma lista de comandos deste terminal\n");

    int probe_status = linenoiseProbe();
    if (probe_status) 
    { 
        printf("Terminal não suporta para essa aplicação .\n");
        linenoiseSetDumbMode(1);
    }
    
}