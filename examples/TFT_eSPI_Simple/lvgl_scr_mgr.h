#pragma once

#ifndef __SCR_MGR_H__
#define __SCR_MGR_H__

/*********************************************************************************
 *                                  INCLUDES
 * *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/*********************************************************************************
 *                                   DEFINES
 * *******************************************************************************/
#define DEF_SCR_MGR_LITTLE_MEM 0

// ARM 结构的单片机使用
// #define SCR_MGR_REG(handle) \
//     __attribute__((used, section("APP_SCREEN"))) const typeof(handle) *ScrMgrScrHandleList_##handle = &(handle)

#define SCR_MGR_REG(handle)

#define SCR_MGR_SCR_TRANS_ANIM_INT 500
#define SCR_MSG_SCR_SWITCH_ANIM LV_SCR_LOAD_ANIM_NONE
#define SCR_MSG_SCR_PUSH_ANIM LV_SCR_LOAD_ANIM_MOVE_TOP
#define SCR_MSG_SCR_POP_ANIM LV_SCR_LOAD_ANIM_MOVE_TOP

/*********************************************************************************
 *                                   MACROS
 * *******************************************************************************/
#ifdef __SCR_MGR_C__
#define DEF_SCR_MGR_EXT
#else
#define DEF_SCR_MGR_EXT extern
#endif

/*********************************************************************************
 *                                  TYPEDEFS
 * *******************************************************************************/
typedef struct scr_mgr_scr_handle_t
{
    uint32_t ScrId;
    lv_obj_t *(*ScrCreate)(lv_obj_t *parent);
    void (*ScrEnter)(void);
    void (*ScrExit)(void);
    void (*ScrDestroy)(void);
} SCR_MGR_SCR_HANDLE_T;

typedef enum scr_mgr_scr_state_e
{
    SCR_MGR_SCR_STATE_IDLE = 0,  /* Not in use */
    SCR_MGR_SCR_STATE_DESTROYED, /* Not active and having been destroyed */
    SCR_MGR_SCR_STATE_CREATED,   /* Created */
    SCR_MGR_SCR_STATE_INACTIVE,  /* Not active */
    SCR_MGR_SCR_STATE_ACTIVE_BG, /* Active but at the background */
    SCR_MGR_SCR_STATE_ACTIVE,    /* Active and at the foreground */
} SCR_MGR_SCR_STATE_E;

typedef struct scr_mgr_scr_stack_handle_t
{
    struct scr_mgr_scr_handle_t *ScrHandle;
    lv_obj_t *ScrObj;
    SCR_MGR_SCR_STATE_E State;
    struct scr_mgr_scr_stack_handle_t *Prev;
    struct scr_mgr_scr_stack_handle_t *Next;
} SCR_MGR_SCR_STACK_HANDLE_T;

/*********************************************************************************
 *                              GLOBAL PROTOTYPES
 * *******************************************************************************/
enum
{
    scr_mgr_id_main = 0,
    scr_mgr_id_clock1,
    scr_mgr_id_clock2,
    scr_mgr_id_max,
};

extern const SCR_MGR_SCR_HANDLE_T gui_main;
extern const SCR_MGR_SCR_HANDLE_T gui_clock1;
extern const SCR_MGR_SCR_HANDLE_T gui_clock2;

//
#define ScrRegister(name, id)                 \
    const SCR_MGR_SCR_HANDLE_T gui_##name = { \
        .ScrId = id,                          \
        .ScrCreate = gui_##name##Create,      \
        .ScrEnter = gui_##name##Enter,        \
        .ScrExit = gui_##name##Exit,          \
        .ScrDestroy = gui_##name##Destory,    \
    };

DEF_SCR_MGR_EXT void ScrMgrInit(void);

/* Clear current stack and switch to new screen */
DEF_SCR_MGR_EXT bool ScrMgrSwitchScr(uint32_t scr_id, bool anim);

DEF_SCR_MGR_EXT bool ScrMgrPushScr(uint32_t scr_id, bool anim);

DEF_SCR_MGR_EXT bool ScrMgrPopScr(bool anim);

/* Pop n screen */
DEF_SCR_MGR_EXT bool ScrMgrPopNScr(uint32_t cnt, bool anim);

DEF_SCR_MGR_EXT bool ScrMgrPopToRoot(bool anim);

DEF_SCR_MGR_EXT void ScrMgrTrimMem(void);

#endif /* __SCR_MGR_H__ */
