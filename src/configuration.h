// file: configuration.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_CONFIGURATION_H
#define NOIA_CONFIGURATION_H

#include "global-functions.h"
#include "input-functions.h"
#include "input-binding.h"

#include <linux/input.h>

static const NoiaConfig scConfig = {
        // Absolute path to background image. Only JPEG supported.
        .background_image_path = "",

        // Transformation type of background image.
        // Available: NOIA_BG_TRANS_REPEAT, NOIA_BG_TRANS_CENTER,
        // NOIA_BG_TRANS_SCALE, NOIA_BG_TRANS_STRETCH,
        .background_image_transform = NOIA_BG_TRANS_REPEAT,

        // Color of background not covered with background image.
        .background_color = {.b=0x21, .g=0x21, .r=0x21, .a=0xFF}
    };

static const NoiaBinding scBindingsCommon[] = {
        // general
        {KEY_ESC,  NOIA_KEY_CTRL|NOIA_KEY_ALT, noia_quit},
        {KEY_F1,   NOIA_KEY_CTRL|NOIA_KEY_ALT, noia_refresh_displays},
    };

static const NoiaBinding scBindingsNormal[] = {
        // stack
        {KEY_ESC, NOIA_KEY_NONE, noia_clean_action},
        // modes
        {KEY_I, NOIA_KEY_NONE, noia_swap_mode_normal_to_insert},
        // actions
        {KEY_F, NOIA_KEY_NONE,  noia_put_focus},
        {KEY_F, NOIA_KEY_SHIFT, noia_put_swap},
        {KEY_M, NOIA_KEY_NONE,  noia_put_move},
        {KEY_R, NOIA_KEY_NONE,  noia_put_resize},
        {KEY_A, NOIA_KEY_NONE,  noia_anchorize},
        {KEY_S, NOIA_KEY_NONE,  noia_stackedize},
        {KEY_V, NOIA_KEY_NONE,  noia_verticalize},
        {KEY_H, NOIA_KEY_NONE,  noia_horizontalize},
        {KEY_P, NOIA_KEY_NONE,  noia_select_trunk},
        // directions
        {KEY_RIGHT,    NOIA_KEY_NONE, noia_right},
        {KEY_LEFT,     NOIA_KEY_NONE, noia_left},
        {KEY_UP,       NOIA_KEY_NONE, noia_up},
        {KEY_DOWN,     NOIA_KEY_NONE, noia_down},
        {KEY_PAGEUP,   NOIA_KEY_NONE, noia_forward},
        {KEY_PAGEDOWN, NOIA_KEY_NONE, noia_back},
        // numbers
        {KEY_MINUS,     NOIA_KEY_NONE, noia_put_number},
        {KEY_0,         NOIA_KEY_NONE, noia_put_number},
        {KEY_1,         NOIA_KEY_NONE, noia_put_number},
        {KEY_2,         NOIA_KEY_NONE, noia_put_number},
        {KEY_3,         NOIA_KEY_NONE, noia_put_number},
        {KEY_4,         NOIA_KEY_NONE, noia_put_number},
        {KEY_5,         NOIA_KEY_NONE, noia_put_number},
        {KEY_6,         NOIA_KEY_NONE, noia_put_number},
        {KEY_7,         NOIA_KEY_NONE, noia_put_number},
        {KEY_8,         NOIA_KEY_NONE, noia_put_number},
        {KEY_9,         NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_0, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_1, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_2, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_3, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_4, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_5, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_6, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_7, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_8, NOIA_KEY_NONE, noia_put_number},
        {KEY_NUMERIC_9, NOIA_KEY_NONE, noia_put_number},
    };

static const NoiaBinding scBindingsInsert[] = {
        // modes
        {KEY_ESC, NOIA_KEY_NONE, noia_swap_mode_insert_to_normal},
        // changing focus
        {KEY_TAB, NOIA_KEY_META,                noia_cicle_history_forward},
        {KEY_TAB, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_cicle_history_back},
        // workspaces
        {KEY_0,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_1,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_2,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_3,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_4,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_5,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_6,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_7,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_8,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_9,         NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_0, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_1, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_2, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_3, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_4, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_5, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_6, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_7, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_8, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_NUMERIC_9, NOIA_KEY_META,                noia_focus_workspace},
        {KEY_0,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_1,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_2,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_3,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_4,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_5,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_6,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_7,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_8,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_9,         NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_0, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_1, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_2, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_3, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_4, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_5, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_6, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_7, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_8, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
        {KEY_NUMERIC_9, NOIA_KEY_META|NOIA_KEY_SHIFT, noia_jump_to_workspace},
    };

#endif // NOIA_CONFIGURATION_H

