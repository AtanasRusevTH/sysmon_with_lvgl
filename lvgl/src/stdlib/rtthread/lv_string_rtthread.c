/**
 * @file lv_string_rtthread.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_STRING == LV_STDLIB_RTTHREAD
#include "../lv_string.h"
#include "../lv_mem.h" /*Need lv_malloc*/
#include <rtthread.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * LV_ATTRIBUTE_FAST_MEM lv_memcpy(void * dst, const void * src, size_t len)
{
    return rt_memcpy(dst, src, len);
}

void LV_ATTRIBUTE_FAST_MEM lv_memset(void * dst, uint8_t v, size_t len)
{
    rt_memset(dst, v, len);
}

void * LV_ATTRIBUTE_FAST_MEM lv_memmove(void * dst, const void * src, size_t len)
{
    return rt_memmove(dst, src, len);
}

size_t lv_strlen(const char * str)
{
    return rt_strlen(str);
}

int32_t lv_memcmp(const void * p1, const void * p2, size_t len)
{
    return rt_memcmp(p1, p2, len);
}

char * lv_strncpy(char * dst, const char * src, size_t dest_size)
{
    return rt_strncpy(dst, src, dest_size);
}

char * lv_strcpy(char * dst, const char * src)
{
    return rt_strcpy(dst, src);
}

int32_t lv_strcmp(const char * s1, const char * s2)
{
    return rt_strcmp(s1, s2);
}

char * lv_strdup(const char * src)
{
    size_t len = lv_strlen(src) + 1;
    char * dst = lv_malloc(len);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len); /*memcpy is faster than strncpy when length is known*/
    return dst;
}

char * lv_strcat(char * dst, const char * src)
{
    return strcat(dst, src);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_RTTHREAD*/