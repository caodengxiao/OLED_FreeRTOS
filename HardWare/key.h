#ifndef __KEY_H__
#define __KEY_H__

// clang-format off
/* =========================== 用户配置 =========================== */
#define KEY_TYPE                1       /* 按键类型 1:GPIO 2:ADC */
#define KEY_NUM                 4       /* 按键数量 */
#define KEY_SCAN_PERIOD         10      /* 按键扫描周期 单位:ms 建议10ms */
#define MAX_CLICK_COUNT         3       /* 最大连击次数(最大三次) */
#define DEBOUNCE_THRESHOLD      2       /* 消抖阈值 连续检测到按键状态变化的次数 */
#define NO_KEY                  0xFF    /* 无按键值 */
static inline void KEY_INIT_FUN(void)   /* 用户gpio初始化接口 */
{
} 
/* =========================== GPIO按键 配置区 =========================== */
#if KEY_TYPE == 1
    #include "gpio.h"
    #define GPIO_KEY_PORT           GPIOA,GPIOA,GPIOA,GPIOA,   /* 按键端口 */
    #define GPIO_KEY_PIN            GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,    /* 按键引脚 */
    #define GPIO_KEY_VALUE          1,2,3,4       /* 按键值*/
    #define GPIO_KEY_POLARITY       0,0,0,0       /* 按键极性 0:低电平有效 1:高电平有效*/
    static const GPIO_TypeDef       *gpio_key_port[KEY_NUM]    = {GPIO_KEY_PORT};
    static const uint16_t           gpio_key_pin[KEY_NUM]      = {GPIO_KEY_PIN};
    static inline uint8_t GPIO_KEY_PIN_READ(uint8_t key) /* 用户gpio读接口 */
    {
        return HAL_GPIO_ReadPin((GPIO_TypeDef *)gpio_key_port[key], gpio_key_pin[key]);
    }
/* =========================== ADC按键 配置区 =========================== */
#elif KEY_TYPE == 2
    #include "System/ADC/sys_adc.h"
    #define ADC_KEY_VALUE           1,2,3,4,5       /* 按键值 */
    #define ADC_KEY_VDDIO           (0xfffL)        /* ADC最大读数4095*/
    #define ADC_KEY_R_UP            100             /* 上拉电阻阻值 单位:0.1K*/
    #define ADC_KEY_RES             0,62,150,240    /* 分压电阻阻值 单位:0.1K*/
    static inline uint16_t ADC_KEY_GET_ADC_VALUE(void) /* 用户adc读接口 */
    {
       return sys_adc_get_value(0);
    }
#endif
// clang-format on

/* =========================== 外部声明 =========================== */

#include "stdint.h"

// 按键事件枚举
typedef enum
{
    KEY_EVENT_NONE = 0,     // 无事件
    KEY_EVENT_DOWN,         // 按键按下
    KEY_EVENT_CLICK,        // 单击
    KEY_EVENT_DOUBLE_CLICK, // 双击
    KEY_EVENT_TRIPLE_CLICK, // 三击
    KEY_EVENT_LONG_PRESS,   // 长按
    KEY_EVENT_HOLD,         // 按住
    KEY_EVENT_UP,           // 按键抬起
} key_event_t;

typedef struct
{
    key_event_t event; // 当前按键事件
    uint8_t value;     // 按键值
} key_state_t;

/**
 * @breif   按键初始化
 * @param   无
 * @retval  无
 */
void key_init(void);

/**
 * @breif   按键扫描
 * @param   无
 * @retval  无
 */
void key_tick(void);

/**
 * @breif   按键获取状态
 * @param   state 按键状态结构体指针
 * @retval  无
 */
void key_get_state(key_state_t *state);

/**
 * @breif   按键扫描设置
 * @param   long_press_time 长按时间 ms
 * @param   hold_press_time 持续按时间 ms
 * @param   multi_click_timeout 多次点击时间 ms
 * @note    推荐500ms长按，1000ms持续按，250ms多次点击
 * @retval  无
 */
void key_period_setting(uint16_t long_press_time, uint16_t hold_press_time, uint16_t multi_click_timeout);

#endif
