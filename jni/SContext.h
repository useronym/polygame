#ifndef SContext_H_INCLUDED
#define SContext_H_INCLUDED


#define DEBUG_INFO
//#define DEBUG_UPDATES_THREADED
//#define DEBUG_PLAYER
#define DEBUG_OBJS
#define DEBUG_GRID
//#define DEBUG_EFFECTS
//#define DEBUG_INPUT

#define MAX_SEED 32768
#define TRACERS_FILENAME "tracers.json"
#define TRACER_EXT ".tbin"
#define ACCEL_SAMPLES 4

#define GUI_TARGET_X 854
#define GUI_TARGET_Y 480

#ifndef _IRR_ANDROID_PLATFORM_
#define DESKTOP_WND_X 854
#define DESKTOP_WND_Y 480
#endif // _IRR_ANDROID_PLATFORM_

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
class Settings;
struct SMaterials;

enum E_SCREEN_SIZE
{
    ESS_SMALL = 0,
    ESS_NORMAL,
    ESS_LARGE,
    ESS_XLARGE
};

enum E_SCREEN_ROTATION
{
    ESR_0 = 0,
    ESR_90,
    ESR_180,
    ESR_270,
};


struct SContext
{
    android_app* App;
    irr::IrrlichtDevice* Device;

    Object* State;
    ObjectManager* ObjManager;

    EffectRenderer* Renderer;
    SMaterials* Mtls;

    irr::core::stringc StoragePath;
    Settings* Sets;
    irr::f32 TimeScale;
    irr::core::vector2df GUIScale;
    irr::core::dimension2du ScreenResolution;
    E_SCREEN_SIZE ScreenSize;
    E_SCREEN_ROTATION ScreenRotation;
};


#endif // SContext_H_INCLUDED
