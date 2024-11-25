#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <NewPing.h>
#include <U8g2lib.h>
#include <TimerFreeTone.h> // BIBLIOTECA PARA CONTROLE DO BUZZER

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#define VL53L0X_ADDRESS 0x30 // ENDEREÇO I2C DO VL53L0X

// DEFINIÇÃO DE PINOS DO HC-SR04
#define TRIGGER_PIN  9
#define ECHO_PIN     10
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// DEFINIÇÃO DE PINOS DO YL-S201
#define FLOW_SENSOR_PIN 2
volatile int pulseCount;
float flowRate;
float totalLiters = 0;
float lastTotalLiters = 0; // VOLUME ANTERIOR ARMAZENADO PARA AJUSTES

const float containerVolume = 2.0; // VOLUME DO RECIPIENTE SETADO PARA 2 LITROS

// PINO DO BUZZER
#define BUZZER_PIN 3
float lastAlertLevel = 0;

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.clearBuffer();

  if (!lox.begin(VL53L0X_ADDRESS)) {
    Serial.println("Falha ao iniciar VL53L0X.");
    while (1);
  }

  pinMode(FLOW_SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), countPulses, RISING); // DEFINIÇÃO DE CONTAGEM DE PULSOS YL-S201

  pinMode(BUZZER_PIN, OUTPUT); // CONFIGURAÇÃO DO PINO DO BUZZER
  Serial.println("Setup concluído.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  float distanceVL53L0X = (measure.RangeStatus != 4) ? measure.RangeMilliMeter / 10.0 : 0; // DISTÂNCIA MEDIDA PELO VL53L0X

  float distanceHC_SR04 = sonar.ping_cm(); // DISTÂNCIA MEDIDA PELO HC-SR04
  float avgDistance = (((distanceVL53L0X + distanceHC_SR04) / 2.0) * 0.9); // MÉDIA DAS DISTÂNCIAS DOS SENSORES HC-SR04 E VL53L0X

  flowRate = pulseCount / 7.5; // CONTAGEM DE PULSOS PARA SENSOR DE VAZÃO YL-S201
  totalLiters += flowRate / 60.0;
  pulseCount = 0;

  float percentageFilled = (totalLiters / containerVolume) * 100; // PORCENTAGEM MEDIDA ATRAVÉS DO VOLUME OBTIDO DOS PULSOS DO SENSOR DE VAZÃO YL-S201
  if (percentageFilled > 100.0) {
    percentageFilled = 100.0; // LIMITA A PORCENTAGEM DE ENCHIMENTO A 100%
  }

  // CÁLCULO DA PORCENTAGEM COM BASE NA MÉDIA DAS DISTÂNCIAS (6CM = 100% CHEIO & 25CM = 0% CHEIO)
  float percentageDistance = 0;
  if (avgDistance >= 6 && avgDistance <= 25) {
    percentageDistance = ((15 - avgDistance) / 10.0) * 100.0;
  } else if (avgDistance < 5) {
    percentageDistance = 100.0;
  } else {
    percentageDistance = 0.0;
  }

  float distancemedia = ((int)percentageDistance + (int)percentageFilled) / 2.0; // MÉDIA ENTRE AS PORCENTAGENS
  if (distancemedia > 100.0) {
    distancemedia = 100.0; // LIMITA A PORCENTAGEM MÉDIA A 100%
  }

  // AJUSTAR O VOLUME APENAS SE O NÍVEL MÉDIO DIMINUIR
  if (distancemedia < lastAlertLevel) {
    totalLiters = (distancemedia / 100.0) * containerVolume; // AJUSTA O VOLUME CONFORME O NÍVEL MÉDIO
  }

  if (totalLiters > containerVolume) {
    totalLiters = containerVolume; // LIMITA O VOLUME MÁXIMO A 2 LITROS
  }

  // ALERTA DO BUZZER PARA NÍVEIS DE ENCHIMENTO 
  if (distancemedia >= 25.0 && distancemedia < 50.0 && lastAlertLevel < 25.0) {
    beepBuzzer(1, 1000); // 1 TOQUE, FREQUÊNCIA 1000 HZ
  } else if (distancemedia >= 50.0 && distancemedia < 75.0 && lastAlertLevel < 50.0) {
    beepBuzzer(2, 1000); // 2 TOQUE, FREQUÊNCIA 1000 HZ
  } else if (distancemedia >= 75.0 && distancemedia < 100.0 && lastAlertLevel < 75.0) {
    beepBuzzer(3, 1000); // 3 TOQUE, FREQUÊNCIA 1000 HZ
  } else if (distancemedia >= 100.0 && lastAlertLevel < 100.0) {
    beepBuzzer(4, 1000); // 4 TOQUE, FREQUÊNCIA 1000 HZ
  }

  // ATUALIZAR O ÚLTIMO NÍVEL MÉDIO REGISTRADO
  lastAlertLevel = distancemedia;

  // EXIBIR VOLUME ACUMULADO 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(7, 20);
  u8g2.print("Volume (flx):");
  u8g2.setCursor(40, 45);
  u8g2.print(totalLiters, 1);
  u8g2.print(" L");
  u8g2.sendBuffer();
  delay(1000);

  // ExXIBIR A PORCENTAGEM MÉDIA
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(13, 20);
  u8g2.print("Nivel Med.:");
  u8g2.setCursor(15, 45);
  u8g2.print(distancemedia, 1);
  u8g2.print("% cheio");
  u8g2.sendBuffer();
  delay(1000);

  Serial.print("Distância média: "); // PLOTAGEM NO SERIAL MONITOR PARA VALIDAÇÃO
  Serial.print(distancemedia);
  Serial.println("%");
}

// FUNÇÃO PARA EMITIR TOQUES NO BUZZER
void beepBuzzer(int times, int frequency) {
  for (int i = 0; i < times; i++) {
    TimerFreeTone(BUZZER_PIN, frequency, 200); // AJUSTA A FREQUÊNCIA
    delay(300); // PAUSA ENTRE OS TOQUES
  }
}

void countPulses() {
  pulseCount++;
}
