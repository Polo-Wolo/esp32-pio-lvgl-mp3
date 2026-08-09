#include "images.h"

const ext_img_desc_t images[6] = {
    { "skip-forward", &img_skip_forward },
    { "placeholder", &img_placeholder },
    { "play", &img_play },
    { "skip-back", &img_skip_back },
    { "heart", &img_heart },
    { "repeat", &img_repeat },
};