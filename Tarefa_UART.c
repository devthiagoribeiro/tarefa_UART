#include <stdio.h>
#include <time.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>
#include <stdlib.h>
#include "hardware/pio.h"
#include "matriz_leds.h"
#include "hardware/timer.h"

//definição das variáveis macros de escopo global
#define ledGreen 11
#define ledBlue 12
#define ledRed 13
#define btnA 5
#define btnB 6
#define btnJoy 22
uint a = 0;
static volatile lastTimeA = 0;
static volatile lastTimeB = 0;
static volatile lastTimeJ = 0;
PIO pio;
uint sm;
bool ledGreenStatus = false;
bool ledBlueStatus = false;
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
struct render_area *frame_area_ptr;

struct render_area start(){
    //inicialização dos gpio's
    gpio_init(ledGreen);
    gpio_set_dir(ledGreen, GPIO_OUT);
    gpio_init(ledBlue);
    gpio_set_dir(ledBlue, GPIO_OUT);
    gpio_init(btnA);
    gpio_set_dir(btnA, GPIO_IN);
    gpio_pull_up(btnA);
    gpio_init(btnB);
    gpio_set_dir(btnB, GPIO_IN);
    gpio_pull_up(btnB);
    gpio_init(btnJoy);
    gpio_set_dir(btnJoy, GPIO_IN);
    gpio_pull_up(btnJoy);
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    return frame_area;
}

//criação das matrizes correspondentes a cada número a ser mostrado na matriz de leds
Matriz_leds_config um = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config zero = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config dois = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config tres = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config quatro = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config cinco = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config seis = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config sete = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config oito = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config nove = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.01, 0.01, 0.01}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config cls = {
            //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
            //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
            {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
            };
Matriz_leds_config* numeros[] = {&zero, &um, &dois, &tres, &quatro, &cinco, &seis, &sete, &oito, &nove};

char *azulOn[8] = {
        //"             "
        "               ",
        "               ",
        "               ",
        "  Led azul ON  ",
        "               ",
        "               ",
        "               ",
        "               "
        };
char *azulOff[8] = {
        //"             "
        "               ",
        "               ",
        "               ",
        " Led azul OFF  ",
        "               ",
        "               ",
        "               ",
        "               "
        };
char *verdeOn[8] = {
        //"             "
        "               ",
        "               ",
        "               ",
        " Led verde ON  ",
        "               ",
        "               ",
        "               ",
        "               "
        };
char *verdeOff[8] = {
        //"             "
        "               ",
        "               ",
        "               ",
        " Led verde OFF ",
        "               ",
        "               ",
        "               ",
        "               "
        };
char *clr[8] = {
        //"             "
        "               ",
        "               ",
        "               ",
        "               ",
        "               ",
        "               ",
        "               ",
        "               "
        };

//rotina de interrupção dos botões
void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t currentTime = to_us_since_boot(get_absolute_time());
    //tratamento debouncing
    if(gpio == btnA && currentTime - lastTimeA > 200000){
        lastTimeA = currentTime;
        ledGreenStatus = !ledGreenStatus;
        printf("%s\n", ledGreenStatus?"Led verde ligado":"Led verde desligado");
        ledGreenStatus?displayOled(verdeOn,count_of(verdeOn), frame_area_ptr):displayOled(verdeOff,count_of(verdeOff), frame_area_ptr);
        gpio_put(ledGreen, ledGreenStatus);
    //tratamento debouncing
    }else if(gpio == btnB && currentTime - lastTimeB > 200000){
        lastTimeB = currentTime;
        ledBlueStatus = !ledBlueStatus;
        ledBlueStatus?displayOled(azulOn,count_of(azulOn), frame_area_ptr):displayOled(azulOff,count_of(azulOff), frame_area_ptr);
        gpio_put(ledBlue, ledBlueStatus);
    }else if(gpio == btnJoy && currentTime - lastTimeJ > 200000){
        lastTimeJ = currentTime;
        imprimir_desenho(cls, pio, sm);
        displayOled(clr,count_of(clr), frame_area_ptr);
    }
}

void displayOled(char *text[8],size_t line_count, struct render_area *frame_area){
    // zera o display inteiro
        uint8_t ssd[ssd1306_buffer_length];
        memset(ssd, 0, ssd1306_buffer_length);
        render_on_display(ssd, frame_area);

        srand(time(NULL));

        restart:

        // Criação do Texto de Exibição

        int y = 0;
        for (uint i = 0; i < line_count; i++)
        {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 8;
        }
        render_on_display(ssd, frame_area);
}

int main()
{
    stdio_init_all();

    //inicialização da matriz de leds
    pio = pio0;
    sm = configurar_matriz(pio);
    
    struct render_area frame_area = start();
    frame_area_ptr = &frame_area;
    
    //inicialização das rotinas de interrupção
    gpio_set_irq_enabled_with_callback(btnA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btnB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btnJoy, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    char c = '-';
    while (true) {
        scanf(" %c", &c);
        if(c >= 48 && c <= 57){
            int i = c - 48;
            imprimir_desenho(*numeros[i], pio, sm); //atualiza o número na matriz de leds
        }else if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)){
            char *text[8];
            for (int i = 0; i < 8; i++) {
                text[i] = malloc(32 * sizeof(char)); // Aloca espaço suficiente para cada string
                if (text[i] == NULL) {
                    printf("Erro ao alocar memória\n");
                }
            }

            // Preenchendo o array de strings com formato
            snprintf(text[0], 32, "               ");
            snprintf(text[1], 32, "               ");
            snprintf(text[2], 32, "   Caractere   ");
            snprintf(text[3], 32, "    lido: %c   ", c);
            snprintf(text[4], 32, "               ");
            snprintf(text[5], 32, "               ");
            snprintf(text[6], 32, "               ");
            snprintf(text[7], 32, "               ");

            displayOled(text,count_of(text), frame_area_ptr);
        }
        sleep_ms(1);
    }
}
