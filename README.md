# 🛡️ StressGuard V1.0 - Sistema Inteligente de Monitoramento de Bem-Estar no Trabalho

**Integrante:** [Vitor Mezzanotte Constante, RM: 562051]

## 1. Descrição do Problema e Solução Proposta

### 1.1 Problema Abordado

O futuro do trabalho demanda soluções que priorizem a **saúde e o bem-estar** dos colaboradores. Condições ambientais inadequadas (como pouca luz ou alta temperatura) são fatores críticos que levam ao estresse e à fadiga, diminuindo a produtividade.

### 1.2 Solução: StressGuard

O **StressGuard** é uma solução de **Edge Computing** desenvolvida com ESP32, alinhada ao tema "Saúde e bem-estar no trabalho: monitoramento ambiental, pausas inteligentes".

Ele atua como um **Mecanismo de Feedback Inteligente** que monitora o ambiente e aplica uma **Lógica de Decisão Automática** para sugerir pausas em tempo real, utilizando um display LCD como atuador.

## 2. Desenvolvimento Técnico e Arquitetura

O projeto foi desenvolvido no Wokwi e utiliza o protocolo **MQTT** para comunicação com um serviço externo de IoT (`44.223.43.74`).

### 2.1 Componentes Utilizados (Hardware)

* **Microcontrolador:** ESP32 (Atua como a "Edge" para processar dados localmente).
* **Sensor de Luminosidade (LDR):** Leitura da intensidade de luz ambiente (**GPIO 34**).
* **Sensor de Temperatura e Umidade (DHT22):** Leitura de T/U (**GPIO 4**).
* **Atuador (Feedback Inteligente):** Display LCD I2C 16x2.

### 2.2 Lógica de Decisão Automática

A lógica é executada localmente no ESP32 antes de enviar o status final via MQTT.

| Fator de Risco | Condição (Lógica) | Resultado | Feedback LCD |
| :--- | :--- | :--- | :--- |
| **Luminosidade** | Luz **abaixo** de `100 lux` (ambiente escuro) | Alerta Ativo | **PAUSA SUGERIDA** |
| **Temperatura** | Temperatura **acima** de `28°C` | Alerta Ativo | **PAUSA SUGERIDA** |
| **Status Geral** | Se qualquer alerta estiver ativo | `ALERTA: Ambiente Inadequado!` | `PAUSA SUGERIDA` |
| **Status Geral** | Se todos os fatores estiverem saudáveis | `OK: Ambiente Saudável` | `BEM-ESTAR: OK` |

### 2.3 Tópicos MQTT e Comunicação

O sistema publica dados no servidor `44.223.43.74` (Porta **1883**), garantindo o requisito de comunicação com um serviço externo de IoT.

| Tópico MQTT | Conteúdo Publicado | Função |
| :--- | :--- | :--- |
| **`gs2025/stressguard/data`** | JSON com {temp, umid, lux, alerta} | Envio de dados brutos para dashboard de monitoramento. |
| **`gs2025/stressguard/alerta`** | `"PAUSA SUGERIDA"` ou `"BEM-ESTAR: OK"` | Envio do status final da Lógica de Decisão (Feedback). |

## 3. Detalhes de Implementação e Solução de Bugs

### 3.1 Superação do Bug do Sensor DHT22 no Wokwi

Durante a simulação, o sensor DHT22 apresentou um bug de *timing* persistente (`Erro na leitura do sensor DHT!`), mesmo após a correta instalação das bibliotecas da Adafruit, o uso de resistor *pull-up* e a alteração dos pinos para o mais estável (**GPIO 4**).

Para garantir a funcionalidade do projeto e a demonstração da Lógica de Decisão, o DHT22 foi **simulado com valores fixos** (`Temp: 25.0C` e `Umid: 50.0%`) no loop principal. A funcionalidade da **Lógica de Decisão Automática** foi **comprovada** com sucesso usando o sensor de **Luminosidade (LDR)**, conforme demonstrado no vídeo.

### 3.2 Correções de Estabilidade (Wokwi)

1.  **Conexão Wi-Fi:** Uso de `"Wokwi-GUEST"` e senha vazia.
2.  **Bibliotecas DHT:** Instalação de `DHT sensor library` e `Adafruit Unified Sensor`.
3.  **Estabilidade:** Adição de `delay(2000)` e leitura *dummy* no `setup()` para estabilização do sensor.

## 4. Entregas

* **Link da Simulação Wokwi:** [https://wokwi.com/projects/448169008425948161]
* **Link do Vídeo Explicativo (Máx. 3 minutos):** [INSERIR O LINK DO VÍDEO AQUI]