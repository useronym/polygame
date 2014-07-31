#ifndef SContext_H_INCLUDED
#define SContext_H_INCLUDED


#define DEBUG_INFO
//#define DEBUG_PLAYER
#define DEBUG_OBJS
//#define DEBUG_GRID
//#define DEBUG_EFFECTS

#define GUI_TARGET_X 854
#define GUI_TARGET_Y 480

#ifndef _IRR_ANDROID_PLATFORM_
#define DESKTOP_WND_X 854
#define DESKTOP_WND_Y 480
#endif // _IRR_ANDROID_PLATFORM_

#define SETTINGS_PATH "settings.json"

#include <irrlicht.h>

class android_app;

namespace irr
{
    class IrrlichtDevice;

    namespace core
    {
        class vector2d<class T>;
    }
}

class Object;
class ObjectManager;
class EffectRenderer;
struct SMaterials;
struct SSettings;

enum E_SCREEN_SIZE
{
    ESS_SMALL = 0,
    ESS_NORMAL,
    ESS_LARGE,
    ESS_XLARGE
};


struct SContext
{
    android_app* App;
    irr::IrrlichtDevice* Device;

    Object* State;
    ObjectManager* ObjManager;

    EffectRenderer* Renderer;
    SMaterials* Mtls;

    SSettings* Settings;
    irr::f32 TimeScale;
    irr::core::vector2df GUIScale;
    E_SCREEN_SIZE ScreenSize;
};


#endif // SContext_H_INCLUDED
