#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_helmet;
extern const lv_img_dsc_t img_helmet__grey_;
extern const lv_img_dsc_t img_reflective_clothing;
extern const lv_img_dsc_t img_reflective_clothing__grey_;
extern const lv_img_dsc_t img_goggles;
extern const lv_img_dsc_t img_goggles_grey_;
extern const lv_img_dsc_t img_toe_cap;
extern const lv_img_dsc_t img_toe_cap_grey_;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[8];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/