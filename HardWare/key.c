#include "key.h"

uint16_t key_long_press_time;     // 长按时间阈值
uint16_t key_hold_press_time;     // 持续按住时间阈值
uint16_t key_multi_click_timeout; // 多击超时时间阈值

static key_state_t key_state = {
    .event = KEY_EVENT_NONE, // 初始状态为用户自定义事件
    .value = NO_KEY          // 初始按键值为无按键
};

#if KEY_TYPE == 1
    static const uint8_t gpio_key_value[KEY_NUM] = {GPIO_KEY_VALUE};
    static const uint8_t gpio_key_polarity[KEY_NUM] = {GPIO_KEY_POLARITY};
#elif KEY_TYPE == 2
    static const uint8_t adc_key_value[KEY_NUM] = {ADC_KEY_VALUE};
    static const uint16_t adc_key_res[KEY_NUM] = {ADC_KEY_RES};
    static uint16_t adc_key_adc_value[KEY_NUM];
#endif

/**
 * @breif   按键扫描设置
 * @param   long_press_time 长按时间 ms
 * @param   hold_press_time 持续按时间 ms
 * @param   multi_click_timeout 多次点击时间 ms
 * @note    推荐500ms长按，1000ms持续按，250ms多次点击
 * @retval  无
 */
void key_period_setting(uint16_t long_press_time, uint16_t hold_press_time, uint16_t multi_click_timeout)
{
    /* 设置按键扫描参数 */
    key_long_press_time = long_press_time / KEY_SCAN_PERIOD;
    key_hold_press_time = hold_press_time / KEY_SCAN_PERIOD;
    key_multi_click_timeout = multi_click_timeout / KEY_SCAN_PERIOD;
}

/**
 * @breif   按键初始化
 * @param   无
 * @retval  无
 */
void key_init(void)
{
    KEY_INIT_FUN();
    key_period_setting(500, 1000, 250); // 设置按键扫描参数 推荐500ms长按，1000ms持续按，250ms多次点击

#if KEY_TYPE == 2

    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (i < KEY_NUM - 1)
            adc_key_adc_value[i] = (uint16_t)((ADC_KEY_VDDIO * adc_key_res[i] / (adc_key_res[i] + ADC_KEY_R_UP)) + (ADC_KEY_VDDIO * adc_key_res[i + 1] / (adc_key_res[i + 1] + ADC_KEY_R_UP))) / 2;
        else
            adc_key_adc_value[i] = (uint16_t)((ADC_KEY_VDDIO * adc_key_res[i] / (adc_key_res[i] + ADC_KEY_R_UP)) + ADC_KEY_VDDIO) / 2;
    }

#endif
}

/**
 * @breif   读取按键GPIO状态
 * @param   无
 * @retval  按键值
 */
static uint8_t key_read(void)
{
#if KEY_TYPE == 1
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (GPIO_KEY_PIN_READ(i) == gpio_key_polarity[i])
            return gpio_key_value[i];
    }
    return NO_KEY;
#elif KEY_TYPE == 2
    uint16_t adc_value = ADC_KEY_GET_ADC_VALUE();
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (adc_value <= adc_key_adc_value[i])
        {
            return adc_key_value[i];
        }
    }
    return NO_KEY;
#endif
}

/**
 * @breif   按键扫描
 * @param   无
 * @retval  无
 */
void key_tick(void)
{
    static uint8_t last_key = NO_KEY;     // 上次扫描的按键值
    static uint8_t debounce_count = 0;    // 消抖计数器
    static uint16_t press_duration = 0;   // 按键持续按下时间
    static uint8_t click_count = 0;       // 连击计数
    static uint8_t pending_key = NO_KEY;  // 等待处理的按键值
    static uint8_t multi_click_timer = 0; // 多击超时计时器

    uint8_t current_key = key_read(); // 读取当前按键值

    /*--- 按键状态机 ---*/
    // 状态1: 按键值变化 (可能抖动或真实按键)
    if (current_key != last_key)
    {
        debounce_count++;

        // 消抖确认：连续 DEBOUNCE_THRESHOLD 个检测周期判断按键值与上一次触发的键值不同
        if (debounce_count >= DEBOUNCE_THRESHOLD)
        {
            // 按键释放事件
            if (current_key == NO_KEY)
            {
                // 长按后释放
                if (press_duration >= key_long_press_time)
                {
                    key_state.event = KEY_EVENT_UP;
                    key_state.value = pending_key;
                }
                // 短按释放 (准备连击检测)
                else
                {
                    click_count++;
                    pending_key = last_key;
                    multi_click_timer = key_multi_click_timeout; // 启动多击超时计时，每次按下重置时间
                }
                press_duration = 0; // 重置按下计时
            }
            // 新按键按下
            else
            {
                pending_key = current_key;
            }

            last_key = current_key; // 更新确认的按键值
        }
    }
    // 状态2: 按键值稳定
    else
    {
        debounce_count = 0; // 重置消抖计数器

        // 按键持续按下处理
        if (current_key != NO_KEY)
        {
            press_duration++;

            // 长按触发
            if (press_duration == key_long_press_time)
            {
                click_count = 0; // 长按发生时清除连击计数
                key_state.event = KEY_EVENT_LONG_PRESS;
                key_state.value = current_key;
            }
            // 长按保持触发
            else if (press_duration >= key_hold_press_time)
            {
                if ((press_duration % 10) == 0)
                {
                    key_state.event = KEY_EVENT_HOLD;
                    key_state.value = current_key;
                }
            }
        }
    }

    /*--- 多击事件检测 ---*/
    if (multi_click_timer > 0)      
    {
        multi_click_timer--;

        // 多击超时处理
        if (multi_click_timer == 0)
        {
            /* 按键事件处理 可以在此处添加多击事件 */
            if (click_count == 1)
                key_state.event = KEY_EVENT_CLICK;
            else if (click_count == 2)
                key_state.event = KEY_EVENT_DOUBLE_CLICK;
            else if (click_count == 3)
                key_state.event = KEY_EVENT_TRIPLE_CLICK;

            key_state.value = pending_key;

            click_count = 0;      // 重置连击计数
            pending_key = NO_KEY; // 清除待处理按键
        }
    }
}

/**
 * @breif   按键获取状态
 * @param   state 按键状态结构体指针
 * @retval  无
 */
void key_get_state(key_state_t *state)
{
    state->event = key_state.event;
    state->value = key_state.value;
    key_state.event = KEY_EVENT_NONE; // 重置事件为用户自定义
    key_state.value = NO_KEY;         // 重置按键值为无按state;
}
