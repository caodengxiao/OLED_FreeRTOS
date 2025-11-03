#include "oled.h"
#include "oled_font.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"

static uint8_t oled_display_buffer[OLED_PAGES][OLED_LIST];

/**
 * @breif   初始化OLED
 * @param   无
 * @retval  无
 */
void oled_init(void)
{
    OLED_INIT_FUNS();
    OLED_WRITE_COMMAND((uint8_t *)oled_init_cmd, sizeof(oled_init_cmd) / sizeof(oled_init_cmd[0]));
    oled_clear_all();
    oled_update_all();
}

/**
 * @breif   设置OLED光标位置
 * @param   page:页码 0-7
 * @param   x:列 0-OLED_LIST
 * @retval  无
 */
void oled_set_cursor(uint8_t page, uint8_t x)
{
    uint8_t cmd[3];
    cmd[0] = oled_cursor_cmd[0] | page;
    cmd[1] = oled_cursor_cmd[1] | ((x & 0xF0) >> 4);
    cmd[2] = oled_cursor_cmd[2] | (x & 0x0F);
    OLED_WRITE_COMMAND(cmd, 3);
}

/**
 * @breif  取反OLED
 * @param   无
 * @retval  无
 */
void oled_reverse_all(void)
{
    uint8_t i, j;
    for (i = 0; i < OLED_PAGES; i++)
    {
        for (j = 0; j < OLED_LIST; j++)
        {
            oled_display_buffer[i][j] ^= 0xFF;
        }
    }
}

/**
 * @breif   取反指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_reverse_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i, j;

    for (i = y; i < y + height; i++) // 遍历指定页
    {
        for (j = x; j < x + width; j++) // 遍历指定列
        {
            if (i < OLED_HEIGHT && j < OLED_WIDTH) // 超出屏幕的内容不显示
            {
                oled_display_buffer[i / OLED_PAGES][j] ^= ~(0x01 << (i % 8)); // 将显存数组指定数据清零
            }
        }
    }
}

/**
 * @breif  清空OLED
 * @param   无
 * @retval  无
 */
void oled_clear_all(void)
{
    uint8_t i, j;
    for (i = 0; i < OLED_PAGES; i++)
    {
        for (j = 0; j < OLED_LIST; j++)
        {
            oled_display_buffer[i][j] = 0x00;
        }
    }
}

/**
 * @breif   清空指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i, j;

    for (i = y; i < y + height; i++) // 遍历指定页
    {
        for (j = x; j < x + width; j++) // 遍历指定列
        {
            if (i < OLED_HEIGHT && j < OLED_WIDTH) // 超出屏幕的内容不显示
            {
                oled_display_buffer[i / OLED_PAGES][j] &= ~(0x01 << (i % 8)); // 将显存数组指定数据清零
            }
        }
    }
}

/**
 * @breif   更新OLED显示
 * @param   无
 * @retval  无
 */
void oled_update_all(void)
{
    uint8_t i;
    for (i = 0; i < OLED_PAGES; i++)
    {
        oled_set_cursor(i, 0);
        OLED_WRITE_DATA(oled_display_buffer[i], OLED_LIST);
    }
}

/**
 * @breif   更新指定区域
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_update_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i;

    for (i = y / OLED_PAGES; i < (y + height - 1) / OLED_PAGES + 1; i++) // 遍历指定页
    {
        if (i < OLED_HEIGHT && x < OLED_WIDTH) // 超出屏幕的内容不显示
        {
            oled_set_cursor(i, x);
            OLED_WRITE_DATA(&oled_display_buffer[i][x], width);
        }
    }
}

/**
 * @breif   显示图像
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   image:图像数据
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @retval  无
 */
void oled_show_image(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height)
{
    uint8_t i, j;
    uint8_t page, shift;

    oled_clear_area(x, y, width, height);
    for (i = 0; i < (height - 1) / OLED_PAGES + 1; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (x + j < OLED_WIDTH)
            {
                page = y / OLED_PAGES;
                shift = y % OLED_PAGES;

                if (page + i < OLED_PAGES)
                {
                    oled_display_buffer[page + i][x + j] |= image[i * width + j] << (shift);
                }

                if (page + i + 1 < OLED_PAGES)
                {
                    oled_display_buffer[page + i + 1][x + j] |= image[i * width + j] >> (OLED_PAGES - shift);
                }
            }
        }
    }
}

/**
 * @breif   显示字符
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   ch:字符
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t ch, uint8_t font_size)
{
    if (font_size == OLED_FONT_6X8) // 字体为宽6像素，高8像素
    {
        /*将ASCII字模库OLED_F6x8的指定数据以6*8的图像格式显示*/
        oled_show_image(x, y, oled_font_6x8[ch - ' '], font_size, 8);
    }
    else if (font_size == OLED_FONT_7X12) // 字体为宽6像素，高12像素
    {
        /*将ASCII字模库OLED_F6x8的指定数据以6*8的图像格式显示*/
        oled_show_image(x, y, oled_font_7x12[ch - ' '], font_size, 12);
    }
    else if (font_size == OLED_FONT_8X16) // 字体为宽8像素，高16像素
    {
        /*将ASCII字模库OLED_F8x16的指定数据以8*16的图像格式显示*/
        oled_show_image(x, y, oled_font_8x16[ch - ' '], font_size, 16);
    }
}

/**
 * @breif   显示字符串包含GBK中文
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   str:字符串
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_string(uint8_t x, uint8_t y, uint8_t *str, uint8_t font_size)
{
    uint8_t char_len = 0;
    char temp_char[5];
    uint16_t index;
    uint8_t width;
    uint8_t height;

    while (*str != '\0')
    {
        if ((*str & 0x80) == 0x00)
        {
            char_len = 1;
            temp_char[0] = *str++;
            temp_char[1] = '\0';
        }
        else // 最高位为1
        {
            char_len = 2;
            temp_char[0] = *str++;
            if (*str == '\0')
                break;
            temp_char[1] = *str++;
            temp_char[2] = '\0';
        }

        if (char_len == 1) // 如果是单字节字符
        {
            oled_show_char(x, y, temp_char[0], font_size);
            x += font_size;
        }
        else // 如果是双字节字符
        {
            const OLED_Character_t *pchar = NULL;
            if (font_size == OLED_FONT_7X12)
            {
                pchar = oled_Cfont_12x12;
                width = 12;
                height = 12;
            }
            else if (font_size == OLED_FONT_8X16)
            {
                pchar = oled_Cfont_16x16;
                width = 16;
                height = 16;
            }
            for (index = 0; strcmp((const char *)pchar[index].Index, "") != 0; index++)
            {
                if (strcmp((const char *)pchar[index].Index, temp_char) == 0)
                {
                    break;
                }
            }
            oled_show_image(x, y, pchar[index].Data, width, height);
            x += width;
        }
    }
}

/**
 * @breif  可变参数格式化字符串
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   font_size:字体大小
 * @param   fmt:格式化字符串
 * @retval  无
 */
void oled_printf(uint8_t x, uint8_t y, uint8_t font_size, const char *fmt, ...)
{
    char str[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(str, sizeof(str), fmt, args);
    va_end(args);
    oled_show_string(x, y, (uint8_t *)str, font_size);
}

/**
 * @breif   在指定位置绘制一个点
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @retval  无
 */
void oled_draw_point(uint8_t x, uint8_t y)
{
    if (x < OLED_WIDTH && y < OLED_HEIGHT)
    {
        oled_display_buffer[y / OLED_PAGES][x] |= (0x01 << (y % 8));
    }
}

/**
 * @breif   在指定位置绘制一个矩形
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   width:宽度 0-OLED_LIST
 * @param   height:高度 0-OLED_HEIGHT
 * @param   fill:是否填充 0-不填充 1-填充
 */
void oled_draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t fill)
{
    uint8_t i, j;
    if (fill)
    {
        for (i = x; i < x + width; i++)
        {
            for (j = y; j < y + height; j++)
            {
                oled_draw_point(i, j);
            }
        }
    }
    else
    {
        for (i = x; i < x + width; i++)
        {
            oled_draw_point(i, y);
            oled_draw_point(i, y + height - 1);
        }
        for (i = y; i < y + height; i++)
        {
            oled_draw_point(x, i);
            oled_draw_point(x + width - 1, i);
        }
    }
}

#if OLED_FONT_GBK_EN

/**
 * @breif   显示中文字符串
 * @param   x:列 0-OLED_LIST
 * @param   y:行 0-OLED_HEIGHT
 * @param   str:字符串
 * @param   font_size:字体大小
 * @retval  无
 */
void oled_show_font_string(uint8_t x, uint8_t y, const uint8_t *str, uint8_t font_size)
{

    const char *font_file = (font_size == OLED_FONT_7X12) ? GBK12_FONT_PATH
                                                          : GBK16_FONT_PATH;

    const uint8_t glyph_bytes = (font_size == OLED_FONT_7X12) ? 24U  /* 12×12 列优先，24 字节 */
                                                              : 32U; /* 16×16 列优先，32 字节 */

    const uint8_t glyph_width = (font_size == OLED_FONT_7X12) ? 12U  /* 12×12 列优先，24 字节 */
                                                              : 16U; /* 16×16 列优先，32 字节 */

    uint8_t font_buf[glyph_bytes];
    uint8_t oled_buf[glyph_bytes];

    FIL f;
    uint8_t res;
    res = f_open(&f, font_file, FA_READ);

    while (*str)
    {
        uint8_t kind;
        if (*str <= 0x7F)
            kind = 0; /* 标准 ASCII */
        if (*str >= 0x81)
            kind = 1; /* GBK 首字节 */

        /* ---------- ASCII ---------- */
        if (kind == 0)
        {
            oled_show_char(x, y, *str, font_size);
            x += font_size;
            ++str;
        }
        /* ---------- GBK16×16 ---------- */
        else if (kind == 1)
        {
            uint8_t high = *str++;
            uint8_t low = *str++;

            uint32_t idx = ((uint32_t)(high - 0x81U) * 190U) +
                           ((uint32_t)(low - ((low < 0x7FU) ? 0x40U : 0x41U))); /* 0x8140 为首字符，公式固定 */

            uint32_t offset = idx * glyph_bytes;

            if (res == FR_OK)
            {
                f_lseek(&f, offset);
                if (f_read(&f, font_buf, glyph_bytes, &(UINT){0}) == FR_OK)
                {
                    for (uint8_t col = 0; col < glyph_width; ++col)
                    {
                        uint8_t byte0 = font_buf[col * 2];     // 低字节 → 行 0~7
                        uint8_t byte1 = font_buf[col * 2 + 1]; // 高字节 → 行 8~11（低 4 位有效）

                        // 上半部分：行 0~7 → 反转位顺序（OLED 高位在下）
                        uint8_t upper = 0;
                        for (uint8_t bit = 0; bit < 8; ++bit)
                            if (byte0 & (1U << bit))
                                upper |= 1U << (7U - bit);
                        oled_buf[col] = upper;

                        // 下半部分：行 8~11 → 只取高字节低 4 位，再反转
                        uint8_t lower = 0;
                        for (uint8_t bit = 0; bit < 8; ++bit)
                            if (byte1 & (1U << bit))
                                lower |= 1U << (7U - bit);
                        oled_buf[glyph_width + col] = lower;
                    }
                    oled_show_image(x, y, oled_buf, glyph_width, glyph_width);
                }
            }
            x += glyph_width;
        }
    }
    f_close(&f);
}
#endif

/**
 * @breif   设置OLED显示参数
 * @param   set:参数类型 1-对比度 2-屏幕翻转X 3-屏幕翻转Y 4-屏幕反色
 * @param   value:参数值
 * @retval  无
 */
void oled_show_setting(uint8_t set, uint8_t value)
{

    uint8_t cmd[2];
    if (set == 1) /* 设置对比度 */
    {
        cmd[0] = 0x81;
        cmd[1] = value;
        OLED_WRITE_COMMAND(cmd, 2);
    }
    else if (set == 2) /* 设置屏幕翻转X */
    {
        if (value == 0)
            cmd[0] = 0xA1;
        else
            cmd[0] = 0xA0;
        OLED_WRITE_COMMAND(cmd, 1);
    }
    else if (set == 3) /* 设置屏幕翻转Y */
    {
        if (value == 0)
            cmd[0] = 0xC8;
        else
            cmd[0] = 0xC0;
        OLED_WRITE_COMMAND(cmd, 1);
    }
    else if (set == 4) /* 设置屏幕反色 */
    {
        if (value == 0)
            cmd[0] = 0xA6;
        else
            cmd[0] = 0xA7;
        OLED_WRITE_COMMAND(cmd, 1);
    }
}
