# Projeto de Controle de Matriz LED com RP2040

## Link pra o vídeo: [AQUI](https://youtu.be/SnG9bbeV3jY)

Projeto para controle de uma matriz LED 5x5 WS2812 utilizando o microcontrolador RP2040 (BitDogLab), implementando:
- Controle de LEDs endereçáveis
- Interface com botões usando interrupções
- Sistema de debounce via software
- Exibição numérica estilo display digital

## 🛠️ Funcionalidades Principais
- **Piscar contínuo do LED vermelho** (5Hz)
- **Incremento/Decremento numérico** com botões físicos
- **Exibição de dígitos 0-9** em formato fixo na matriz
- **Mapeamento físico da matriz** para disposição correta dos LEDs
- **Controle de brilho** ajustável

## 📋 Requisitos de Hardware
- Placa BitDogLab com RP2040
- Matriz LED 5x5 WS2812 (GPIO7)
- 2 botões (GPIO5 e GPIO6)
- LED RGB (GPIO11-13)
- Resistores de pull-up internos habilitados

## ⚙️ Instalação
1. Clone o repositório:
    ```bash
    git clone https://github.com/jonathasporto/Numbers_Matrix.git
    ```

2. Configure o ambiente Pico SDK:
    
3. Compile o projeto:

4. Carregue o arquivo `.uf2` na placa

## 🕹️ Uso
1. **LED Vermelho**: Pisca automaticamente a 5Hz
2. **Botão A**: Incrementa o número exibido
3. **Botão B**: Decrementa o número exibido
4. **Matriz LED**: Mostra o valor atual (0-9) com wrap-around

## 🔧 Personalização
- Ajuste o brilho: Modifique `BRIGHTNESS` no código
- Altere os padrões dos números: Edite o array `digits`
- Modifique os efeitos: Adapte a função `update_display`


## 🔗 Recursos
- [Documentação RP2040](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [Especificação WS2812](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf)

> Desenvolvido para estudo de sistemas embarcados e controle de hardware de baixo nível