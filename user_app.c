#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"


#include <string.h>

// Mensagens entre tarefas
typedef enum { MSG_SENSORS = 1, MSG_BAT_LOW = 2 } msg_type_t;

typedef struct {
    uint8_t type;     // MSG_SENSORS
    uint16_t gyro;    // valor bruto do ADC
    uint16_t accel;   // valor bruto do ADC
} sensor_msg_t;

// Variável compartilhada com velocidades dos motores
typedef struct {
    uint8_t m1, m2, m3, m4; // 0..255 (mapeado para PWM duty)
} motor_speeds_t;

static motor_speeds_t g_speeds;
static mutex_t g_speeds_mutex;
static pipe_t g_pipe; // fila de mensagens sensores/bateria -> central
static volatile uint8_t g_flight_enabled = 1;

static void isr_button_toggle(void)
{
    // Alterna modo de voo via botão externo (INT0)
    g_flight_enabled = !g_flight_enabled;
}

void config_app(void)
{
    // ADC: CH0 (gyro), CH1 (accel), CH2 (battery)
    set_channel(CHANNEL_0);
    set_port(AN02); // habilita AN0..AN2 como analógicos
    // Importante: TAD >= 0,7us. Com FOSC=8MHz, use FOSC32 (TAD=4us) ou FOSC64 (TAD=8us)
    config_adc(TAD12, FOSC32);
    adc_go(1);

    // PWM em RD1..RD4
    pwm_init(255);

    // Interrupção externa INT0 em RB0
    exti_init(1); // borda de subida
    exti_set_callback(isr_button_toggle);
    exti_enable(1);

    // IO indicadores (opcional)
    TRISDbits.RD0 = 0; // LED status

    // Sincronização e comunicação
    mutex_init(&g_speeds_mutex);
    create_pipe(&g_pipe);

    // Inicializa velocidades com zero
    g_speeds.m1 = g_speeds.m2 = g_speeds.m3 = g_speeds.m4 = 0;

    // Exporta símbolos das tarefas para o montador
    asm("GLOBAL _task_motors, _task_sensors, _task_battery, _task_central");
}

static uint16_t read_adc_channel(channel_t ch)
{
    set_channel(ch);
    return (uint16_t)adc_read();
}

// Tarefa: controle dos motores (consome variável compartilhada)
TASK task_motors(void)
{
    while (1) {
        // Lê velocidades sob proteção de mutex
        mutex_lock(&g_speeds_mutex);
        motor_speeds_t local = g_speeds;
        mutex_unlock(&g_speeds_mutex);

        if (!g_flight_enabled) {
            local.m1 = local.m2 = local.m3 = local.m4 = 0;
        }

        // Atualiza PWM (duty 0..255)
        pwm_set_duty(PWM_CH1, local.m1);
        pwm_set_duty(PWM_CH2, local.m2);
        pwm_set_duty(PWM_CH3, local.m3);
        pwm_set_duty(PWM_CH4, local.m4);

        os_delay(5); // atualiza ~periodicamente sem monopolizar CPU
    }
}

// Tarefa: leitura dos sensores (gyro e accel) e envio para central via pipe
TASK task_sensors(void)
{
    sensor_msg_t msg;
    msg.type = MSG_SENSORS;

    while (1) {
        msg.gyro  = read_adc_channel(CHANNEL_0);
        msg.accel = read_adc_channel(CHANNEL_1);

        pipe_write_block(&g_pipe, (const char*)&msg, sizeof(msg));
        os_delay(10);
    }
}

// Tarefa: monitoramento da bateria (ADC CH2)
TASK task_battery(void)
{
    const uint16_t LOW_THRESHOLD = 300;
    uint8_t last_low = 0;

    while (1) {
        uint16_t bat = read_adc_channel(CHANNEL_2);
        uint8_t is_low = (bat < LOW_THRESHOLD);
        if (is_low && !last_low) {
            uint8_t type = MSG_BAT_LOW;
            pipe_write_block(&g_pipe, (const char*)&type, sizeof(type));
            last_low = 1;
        } else if (!is_low) {
            last_low = 0;
        }
        os_delay(20);
    }
}

// Tarefa: controle central (processa mensagens e calcula velocidades dos motores)
TASK task_central(void)
{
    // estratégia simples: mapeia gyro/accel (0..1023) para ajustes 0..255
    sensor_msg_t smsg;
    uint8_t mtype;
    while (1) {
        // Peek no próximo byte para decidir o tipo (pipe é FIFO de bytes)
        // Lê primeiro um byte (tipo)
        pipe_read_block(&g_pipe, (char*)&mtype, sizeof(mtype));
        if (mtype == MSG_SENSORS) {
            // Já lemos 1 byte, precisamos ler o restante de sensor_msg_t
            // Copia o tipo e lê o restante
            smsg.type = mtype;
            pipe_read_block(&g_pipe, ((char*)&smsg) + 1, sizeof(smsg) - 1);

            // Normaliza 0..1023 para 0..255
            uint8_t g = (uint8_t)(smsg.gyro >> 2);
            uint8_t a = (uint8_t)(smsg.accel >> 2);

            // Heurística: base 128 com correção por gyro/accel
            int16_t base = 128;
            int16_t m1 = base + (int16_t)g - (int16_t)a;
            int16_t m2 = base - (int16_t)g - (int16_t)a;
            int16_t m3 = base + (int16_t)g + (int16_t)a;
            int16_t m4 = base - (int16_t)g + (int16_t)a;

            // Satura 0..255
            #define SAT(x) ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))
            motor_speeds_t next = {
                (uint8_t)SAT(m1), (uint8_t)SAT(m2), (uint8_t)SAT(m3), (uint8_t)SAT(m4)
            };

            mutex_lock(&g_speeds_mutex);
            g_speeds = next;
            mutex_unlock(&g_speeds_mutex);
        } else if (mtype == MSG_BAT_LOW) {
            // Reduz as velocidades para pouso/retorno
            mutex_lock(&g_speeds_mutex);
            g_speeds.m1 = g_speeds.m2 = g_speeds.m3 = g_speeds.m4 = 60; // valor seguro baixo
            mutex_unlock(&g_speeds_mutex);
        } else {
            // mensagem desconhecida: descarta
        }

        // Sinaliza atividade
        LATDbits.LD0 = ~PORTDbits.RD0;
        os_yield();
    }
}
