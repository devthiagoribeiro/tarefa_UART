# Projeto: Comunicação Serial e Controle de Hardware na BitDogLab

## 📌 Descrição
Este projeto tem como objetivo explorar a comunicação serial UART e I2C utilizando a placa BitDogLab. As funcionalidades implementadas incluem:

- Exibição de caracteres enviados via Serial Monitor no display OLED SSD1306.
- Exibição de símbolos na matriz de LEDs WS2812 ao digitar números de 0 a 9.
- Controle do LED RGB utilizando botões físicos com interrupções e debounce.
- Feedback visual e textual no display e via Serial Monitor.

## 🎥 Demonstração
[https://youtu.be/q_wpexEg35I]

## 🛠 Requisitos
- Microcontrolador **Raspberry pi pico w**
- Display **SSD1306** (I2C)
- Matriz de LEDs **WS2812** (GPIO 7)
- LED RGB (GPIOs 11, 12 e 13)
- Botões A e B (GPIOs 5 e 6)
- **VS Code** com extensão para **RP2040**
- Biblioteca para comunicação I2C e controle de display
- Biblioteca de controle de LEDs WS2812

## 🚀 Como executar o projeto

### 1️⃣ Configuração do ambiente
1. Instale o **VS Code** e configure o suporte para o **RP2040**.
2. Clone este repositório:
   ```sh
   git clone https://github.com/devthiagoribeiro/tarefa_UART
   cd tarefa_UART
   ```
3. Abra o projeto no **VS Code** e configure a placa **BitDogLab** como destino de compilação.

### 2️⃣ Upload do código
1. Conecte a **BitDogLab** ao computador via USB.
2. Compile e carregue o código na placa.
3. Abra o **Serial Monitor** para interagir com o sistema.

### 3️⃣ Uso das funcionalidades
- **Entrada Serial**: Digite um caractere no **Serial Monitor** e veja-o no display.
- **Matriz de LEDs**: Digite um número de 0 a 9 para exibir um símbolo correspondente.
- **Botão A**: Pressione para alternar o LED **verde** e exibir status no display e Serial Monitor.
- **Botão B**: Pressione para alternar o LED **azul** e exibir status no display e Serial Monitor.

## 📝 Notas
- Todas as interrupções são tratadas corretamente para garantir resposta rápida.
- Implementação de debounce para evitar leituras falsas dos botões.
- Código estruturado e comentado para facilitar a compreensão.