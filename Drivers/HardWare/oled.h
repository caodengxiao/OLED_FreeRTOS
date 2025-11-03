#ifndef __OLED_H_
#define __OLED_H_

/* =========================== 用户配置 =========================== */

#include "gpio.h"                       /*用户头文件*/
#include "spi.h"                        /*用户头文件*/
static inline void OLED_INIT_FUNS(void) /* OLED初始化接口 */
{
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_Delay(100);
}
static inline void OLED_WRITE_DATA(uint8_t *data, uint16_t len) /* OLED写数据接口 */
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, data, len, 1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
}
static inline void OLED_WRITE_COMMAND(uint8_t *command, uint16_t len) /* OLED写命令接口 */
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, command, len, 1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
}
// clang-format off
#define OLED_PAGES              8   /* 8页 */
#define OLED_LIST               128 /* 128列 */

#define OLED_WIDTH              128 /* 128列 */
#define OLED_HEIGHT             64  /* 64行 */

#define OLED_FONT_6X8           6   /* 6x8字体 */
#define OLED_FONT_7X12          7   /* 7x12字体 */
#define OLED_FONT_8X16          8   /* 8x16字体 */

#define OLED_FONT_GBK_EN        0   /* 1-使能GBK中文显示 0-关闭GBK中文显示 */

#if OLED_FONT_GBK_EN
    #define GBK12_FONT_PATH         "0:/SYSTEM/FONT/GBK12.FON"  // GBK12字体路径
    #define GBK16_FONT_PATH         "0:/SYSTEM/FONT/GBK16.FON"  // GBK16字体路径
    #include "Middlewares/FATFS/ff.h"   // 文件系统 
#endif

static const uint8_t oled_init_cmd[] = {
    0xAE, // 设置显示开启/关闭，0xAE关闭，0xAF开启
    0xD5, // 设置显示时钟分频比/振荡器频率
    0x80, // 0x00~0xFF
    0xA8, // 设置多路复用率
    0x3F, // 0x0E~0x3F
    0xD3, // 设置显示偏移
    0x00, // 0x00~0x7F
    0x40, // 设置显示开始行，0x40~0x7F
    0xA1, // 设置左右方向，0xA1正常，0xA0左右反置
    0xC8, // 设置上下方向，0xC8正常，0xC0上下反置
    0xDA, // 设置COM引脚硬件配置
    0x12, // 0x02~0x07
    0x81, // 设置对比度
    0x55, // 0x00~0xFF
    0xD9, // 设置预充电周期
    0xF1, // 0x00~0xFF
    0xDB, // 设置VCOMH取消选择级别
    0x30, // 0x00~0x3F
    0xA4, // 设置整个显示打开/关闭
    0xA6, // 设置正常/反色显示，0xA6正常，0xA7反色
    0x8D, // 设置充电泵
    0x14, // 0x10
    0xAF, // 开启显示
};

static const uint8_t oled_cursor_cmd[] = {
    0xB0, // 设置页地址
    0x10, // 设置显示开始列高4位
    0x00, // 设置显示开始列低4位
};
// clang-format on

/* =========================== 外部声明 =========================== */

#include "stdint.h"

/**
 * @breif   初始化OLED
 * @param   无
 * @retval  无
 */
void oled_init(void);

/**
 * @breif   设置OLED光标位置
 * @param   page:页码 0-7
 * @param   x:列 0-OLED_LIST
 * @retval  无
 */
void oled_set_cursor(uint8_t page, uint8_t x);

/**
 * @breif  取反OLED
 * @param   无
 * @retval  无
 */
void oled_reverse_all(void);

/**
 * @breif   取反指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_reverse_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

/**
 * @breif  清空OLED
 * @param   无
 * @retval  无
 */
void oled_clear_all(void);

/**
 * @breif   更新OLED显示
 * @param   无
 * @retval  无
 */
void oled_update_all(void);

/**
 * @breif   清空指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

/**
 * @breif   更新指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_update_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

/**
 * @breif   显示图像
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   image:图像数据
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_show_image(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height);

/**
 * @breif   显示字符
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   ch:字符
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t font_size);

/**
 * @breif   显示字符串包含GBK中文
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   str:字符串
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_string(uint8_t x, uint8_t y, uint8_t *str, uint8_t font_size);

/**
 * @breif  可变参数格式化字符串
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   font_size:字体大小
 * @param   fmt:格式化字符串
 * @retval  无
 */
void oled_printf(uint8_t x, uint8_t y, uint8_t font_size, const char *fmt, ...);

/**
 * @breif   在指定位置绘制一个点
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @retval  无
 */
void oled_draw_point(uint8_t x, uint8_t y);

/**
 * @breif   在指定位置绘制一个矩形
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @param   fill:是否填充 0-不填充 1-填充
 */
void oled_draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t fill);

/**
 * @breif   显示中文字符串
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   str:字符串
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_font_string(uint8_t x, uint8_t y, const uint8_t *str, uint8_t font_size);

/**
 * @breif   设置OLED显示参数
 * @param   set:参数类型 1-对比度 2-屏幕翻转X 3-屏幕翻转Y 4-屏幕反色
 * @param   value:参数值
 * @retval  无
 */
void oled_show_setting(uint8_t set, uint8_t value);

#endif
