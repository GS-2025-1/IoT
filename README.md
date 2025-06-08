
# 🚀 Projeto "Alaga Menos" — Publicação de Dados no ThingSpeak

Este projeto utiliza um **ESP32** e sensores ultrassônicos para monitorar a altura da água em 3 setores e enviar os dados para um painel no **ThingSpeak**.

### 🛠️ Tecnologias utilizadas

- **ESP32** (físico ou Wokwi para testes)
- **MQTT** com `mqtt3.thingspeak.com`
- Fallback com **HTTP REST API** do ThingSpeak
- Bibliotecas:
  - `WiFi.h`
  - `WiFiClientSecure.h`
  - `PubSubClient.h`
  - `HTTPClient.h`

---

## ⚙️ Funcionalidades

✅ Leitura de sensores ultrassônicos em 3 setores  
✅ Publicação dos dados no ThingSpeak via **MQTT**  
✅ Fallback automático via **HTTP REST** se o MQTT falhar  
✅ Atualização do painel do ThingSpeak em tempo real

---

## 📋 Requisitos

- **Placa ESP32 física** (recomendado) ou ambiente de simulação (Wokwi)
- **Conta gratuita no [ThingSpeak](https://thingspeak.com/)**
- Canal criado no ThingSpeak com pelo menos:
  - Field 1 → Setor 1
  - Field 2 → Setor 2
  - Field 3 → Setor 3

---

## 🔑 Configuração necessária

No arquivo `.ino` ou no `main.cpp`, configurar os seguintes parâmetros:

```cpp
const char* channelID = "SEU_CHANNEL_ID";
const char* mqttUser = channelID; // Channel ID como username MQTT
const char* mqttPassword = "SUA_API_WRITE_KEY"; // API Write Key (aba API Keys)
const char* thingspeak_write_key = "SUA_API_WRITE_KEY"; // mesma API Write Key
```

---

## 📥 Instalação das bibliotecas

Instale as seguintes bibliotecas:

### No **PlatformIO** (recomendado):

```ini
lib_deps =
    knolleary/PubSubClient
    bblanchon/ArduinoJson
    bblanchon/ArduinoHttpClient
```

### No **Arduino IDE**:

- WiFi (já incluso no ESP32 core)
- WiFiClientSecure (já incluso no ESP32 core)
- PubSubClient → instale via Gerenciador de Bibliotecas
- HTTPClient → já incluso no ESP32 core

---

## 🚀 Como rodar

### Opção 1 — **ESP32 físico** (recomendado):

1. Conecte a placa ESP32 via USB.
2. Faça upload do código.
3. Abra o Serial Monitor → verifique:

```
Conectando ao WiFi...
Conectando ao MQTT...
Payload enviado ao ThingSpeak via MQTT: field1=...&field2=...&field3=...
```

### Opção 2 — **Wokwi** (para testes):

⚠️ No Wokwi, o MQTT pode falhar por limitação de TLS, mas o fallback REST vai garantir que o painel atualize.

1. Simule o código no Wokwi.
2. Verifique no Serial Monitor:

```
FALHA na publicação MQTT! Tentando via HTTP REST...
Resposta do servidor: 200
```

---

## 🖥️ Visualização no painel ThingSpeak

1. Abra seu canal no ThingSpeak.
2. Vá em **Private View** ou **Public View**.
3. Configure os gráficos para mostrar:
   - Field 1 → Altura da água Setor 1
   - Field 2 → Altura da água Setor 2
   - Field 3 → Altura da água Setor 3
   - Eixo Y → 0 a 300 cm (ou conforme seu sensor)

---

## 📝 Notas importantes

- O código implementa uma lógica **robusta**: tenta MQTT → fallback para REST automaticamente.
- No Wokwi, a falha no MQTT é esperada e não indica erro no código.
- No ESP32 real → MQTT deve funcionar normalmente.

---

## 👨‍💻 Autores

**Projeto Alaga Menos**  
Grupo Impacto Zero — Global Solution FIAP 2025/1
