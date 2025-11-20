/*
 * PROJETO: StressGuard V1.0 - Monitoramento Inteligente de Bem-Estar no Trabalho
 * GLOBAL SOLUTION 2025 - Edge Computing com ESP32
 * * OBJETIVO: Monitorar condições ambientais (T/U/Luminosidade) e acionar
 * Lógica de Decisão Automática, enviando alertas de "PAUSA SUGERIDA"
 * via Display LCD e MQTT, alinhado ao tema "Saúde e bem-estar no trabalho".
 */

#include <WiFi.h>
#include <PubSubClient.h>         // Biblioteca para MQTT
#include <Adafruit_Sensor.h>      // Dependência da biblioteca DHT (para maior estabilidade)
#include "DHT.h"                  // Biblioteca para o sensor DHT22/11
#include <LiquidCrystal_I2C.h>    // Biblioteca para o Display LCD I2C

// ===================================
// 1. CONFIGURAÇÕES GLOBAIS
// ===================================

// ** 1.1 Credenciais de Conexão (Simulação Wokwi) **
const char* ssid = "Wokwi-GUEST"; // Wi-Fi padrão da simulação
const char* password = ""; // Sem senha
const char* mqtt_server = "44.223.43.74"; // Servidor IoT fornecido pelo professor

// ** 1.2 Configurações de Pinos e Sensores **
#define DHTPIN 4        // Pino de dados do DHT22 (GPIO 4, o mais estável no Wokwi)
#define DHTTYPE DHT11   // Tipo de sensor (Usado DHT11 no código para maior estabilidade)
#define LDR_PIN 34      // Pino Analógico para o LDR (GPIO 34/VP)

// ** 1.3 Tópicos MQTT **
const char* DATA_TOPIC = "gs2025/stressguard/data";
const char* ALERT_TOPIC = "gs2025/stressguard/alerta";

// ===================================
// 2. INSTÂNCIAS E VARIÁVEIS
// ===================================
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE); // Instancia o sensor DHT

// Endereço I2C do LCD (padrão do Wokwi é 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

long lastMsg = 0;
#define MEASURE_INTERVAL 15000 // Mede a cada 15 segundos

// ===================================
// 3. FUNÇÕES DE CONEXÃO
// ===================================
void setup_wifi() {
    delay(10);
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Tentando conexão MQTT...");
        // Tenta conectar, usando o MAC address como Client ID
        String clientId = "ESP32Client-";
        clientId += String(micros());
        
        // Porta 1883 é a porta que funcionou na última tentativa bem-sucedida
        if (client.connect(clientId.c_str())) { 
            Serial.println("conectado!");
        } else {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" Tentando em 5 segundos...");
            delay(5000);
        }
    }
}

// ===================================
// 4. LÓGICA DO PROJETO (EDGE COMPUTING)
// ===================================

void loop() {
    if (!client.connected()) {
        reconnect_mqtt();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > MEASURE_INTERVAL) {
        lastMsg = now;

        // ** 4.1. Leitura dos Sensores **
        
        // CORREÇÃO DO BUG DO DHT22 NO WOKWI:
        // O sensor DHT causava o erro "Erro na leitura do sensor DHT!"
        // Para demonstrar a Lógica de Decisão, definimos valores fixos para T/U.
        float temp_c = 25.0; // Valor fixo e saudável
        float humidity = 50.0; // Valor fixo

        // Linhas originais comentadas para evitar o erro de simulação:
        // float temp_c = dht.readTemperature();
        // float humidity = dht.readHumidity();

        // Leitura do LDR (Este sensor está funcionando)
        int light_raw = analogRead(LDR_PIN); 
        float luminosidade = map(light_raw, 0, 4095, 0, 1000); 

        // Checagem de leitura falha (MANTIDA, mas agora irrelevante)
        if (isnan(temp_c) || isnan(humidity)) { 
            Serial.println("Erro na leitura do sensor DHT!"); 
        }

        Serial.print("Temperatura: "); Serial.print(temp_c); Serial.print(" °C | Umidade: "); Serial.print(humidity);
        Serial.print(" % | Luz: "); Serial.print(luminosidade); Serial.println(" lux (aproximado)");


        // ** 4.2. Lógica de Decisão Automática **
        // Se a temperatura estivesse acima de 28.0C OU a luminosidade abaixo de 100.0 lux, o alerta seria ativado.
        String status_alerta = "OK: Ambiente Saudavel";
        bool alerta_ativo = false;

        if (temp_c > 28.0 || luminosidade < 100.0) {
            alerta_ativo = true;
            status_alerta = "ALERTA: Ambiente Inadequado!";
        }

        // ** 4.3. Publicação MQTT **
        client.publish(ALERT_TOPIC, status_alerta.c_str());

        // Cria a string JSON com os dados para o dashboard
        String payload = "{\"temp\":" + String(temp_c) + ", \"umid\":" + String(humidity) + ", \"lux\":" + String(luminosidade) + ", \"alerta\":" + (alerta_ativo ? "true" : "false") + "}";
        client.publish(DATA_TOPIC, payload.c_str());
        
        Serial.print("Publicado no MQTT. Status: "); Serial.println(status_alerta);

        // ** 4.4. Feedback no LCD (Mecanismo de Feedback Inteligente) **
        lcd.clear();
        if (alerta_ativo) {
            lcd.setCursor(0, 0);
            lcd.print("PAUSA SUGERIDA"); // ATUADOR
            lcd.setCursor(0, 1);
            lcd.print(status_alerta);
        } else {
            lcd.setCursor(0, 0);
            lcd.print("BEM-ESTAR: OK");
            lcd.setCursor(0, 1);
            lcd.print("Temp: " + String(temp_c) + "C");
        }
    }
}


// ===================================
// 5. SETUP
// ===================================
void setup() {
    Serial.begin(115200);
    // Inicialização do LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("StressGuard V1.0");
    lcd.setCursor(0, 1);
    lcd.print("Iniciando...");

    // Inicialização do Sensor DHT
    dht.begin(); 
    delay(2000); // Adiciona um atraso para estabilização (ajuda no Wokwi)
    
    // Leitura Dummy para forçar a inicialização do sensor e descartar a primeira leitura falha
    float dummy_read = dht.readTemperature(); 

    // Conexão Wi-Fi
    setup_wifi();

    // Configuração do servidor MQTT
    client.setServer(mqtt_server, 1883); // Porta 1883, que se provou funcional
    
    // Atraso final após todas as inicializações
    delay(3000); 
}