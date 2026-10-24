#include <Arduino.h>
#include "driver/twai.h"
#include "wltp_data.h"

// Hardware CAN ESP32 (Transceiver SN65HVD230)
#define CAN_TX_PIN   GPIO_NUM_25
#define CAN_RX_PIN   GPIO_NUM_26
#define CAN_ID_PLANT 0x100

// Paramètres physiques cellule Li-ion
#define C_NOMINAL_AS   (2.5f * 3600.0f) // 9000 As (2.5 Ah)
#define R0_OHM         0.020f
#define R1_OHM         0.015f
#define C1_FARAD       1200.0f
#define DT_SEC         0.100f           // Période 100 ms (10 Hz)

typedef enum {
    FAULT_NONE = 0,
    FAULT_OVERVOLTAGE,
    FAULT_OVERTEMP,
    FAULT_UNDERVOLTAGE,
    FAULT_OVERCURRENT,
} FaultMode_t;

static float soc_plant = 0.80f;
static float v_rc = 0.0f;
static float temp_plant = 25.0f;
static uint16_t wltp_step = 0;
static FaultMode_t fault_injection = FAULT_NONE;
static unsigned long prev_millis = 0;

static inline float compute_ocv(float soc) {
    return 3.0f + 1.15f * soc + 0.1f * (soc * soc);
}

static bool twai_bus_init(void) {
    twai_general_config_t g_conf = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_conf = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_conf = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_conf, &t_conf, &f_conf) != ESP_OK) {
        return false;
    }
    return (twai_start() == ESP_OK);
}

static void send_battery_frame(float voltage, float current, float temperature) {
    twai_message_t frame;
    frame.identifier = CAN_ID_PLANT;
    frame.extd = 0;
    frame.data_length_code = 5;

    // Mise à l'échelle pour le bus CAN
    uint16_t v_mv = (uint16_t)(voltage * 1000.0f); // Millivolts
    int16_t  i_ca = (int16_t)(current * 100.0f);   // Centiampères (évite le débordement int16_t)
    int8_t   t_c  = (int8_t)temperature;           // °C

    frame.data[0] = (uint8_t)(v_mv >> 8);
    frame.data[1] = (uint8_t)(v_mv & 0xFF);
    frame.data[2] = (uint8_t)(i_ca >> 8);
    frame.data[3] = (uint8_t)(i_ca & 0xFF);
    frame.data[4] = (uint8_t)t_c;

    twai_transmit(&frame, pdMS_TO_TICKS(5));
}

static void process_serial_commands(void) {
    if (!Serial.available()) return;

    char key = Serial.read();
    switch (key) {
        case '1':
            fault_injection = FAULT_OVERVOLTAGE;
            Serial.println("[FAULT] Injection OVP -> V = 4.30V");
            break;
        case '2':
            fault_injection = FAULT_OVERTEMP;
            Serial.println("[FAULT] Injection OTP -> T = 60.0°C");
            break;
        case '3':  
            fault_injection = FAULT_UNDERVOLTAGE;
            Serial.println("[FAULT] Injection UVP -> V = 2.50V");
            break;
        case '4':  
            fault_injection = FAULT_OVERCURRENT;
            Serial.println("[FAULT] Injection OCP -> I = 35.0A");
            break;
        case '0':
            fault_injection = FAULT_NONE;
            Serial.println("[NOMINAL] Conditions nominales rétablies");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("--- BMS HIL Testbench Node ESP32 ---");

    if (!twai_bus_init()) {
        Serial.println("[ERR] Échec d'initialisation du contrôleur CAN TWAI");
        while (1) { delay(1000); }
    }

    Serial.println("[OK] CAN 500 kbps actif");
    Serial.println("Commandes : '0'=Nominal | '1'=OVP | '2'=OTP | '3'=UVP | '4'=OCP");

    prev_millis = millis();
}

void loop() {
    process_serial_commands();

    unsigned long current_millis = millis();
    if (current_millis - prev_millis < 100) {
        return;
    }
    prev_millis = current_millis;

    // 1. Profil de courant WLTP
    float I_current = wltp_current[wltp_step];
    wltp_step = (wltp_step + 1) % WLTP_TOTAL_SECONDS;

    // 2. Injection défaut surintensité
    if (fault_injection == FAULT_OVERCURRENT) {
        I_current = 35.0f;
    }

    // 3. Intégration Coulomb Counting
    soc_plant -= (I_current * DT_SEC) / C_NOMINAL_AS;
    if (soc_plant < 0.0f) soc_plant = 0.0f;
    if (soc_plant > 1.0f) soc_plant = 1.0f;

    // 4. Dynamique RC
    float exp_factor = exp(-DT_SEC / (R1_OHM * C1_FARAD));
    v_rc = v_rc * exp_factor + R1_OHM * (1.0f - exp_factor) * I_current;

    // 5. Tension de cellule
    float v_cell = compute_ocv(soc_plant) - (R0_OHM * I_current) - v_rc;

    // 6. Dynamique thermique
    temp_plant += 0.002f * (R0_OHM * I_current * I_current) - 0.001f * (temp_plant - 25.0f);

    // 7. Overrides des fautes
    if (fault_injection == FAULT_OVERVOLTAGE) {
        v_cell = 4.30f;
    } else if (fault_injection == FAULT_UNDERVOLTAGE) {
        v_cell = 2.20f;
    } else if (fault_injection == FAULT_OVERTEMP) {
        temp_plant = 60.0f;
    }

    // 8. Transmission CAN
    send_battery_frame(v_cell, I_current, temp_plant);

    Serial.printf("[t=%04ds] V=%.3fV | I=%6.2fA | T=%.1fC | SOC=%.2f%%\n",
                  wltp_step, v_cell, I_current, temp_plant, soc_plant * 100.0f);
}