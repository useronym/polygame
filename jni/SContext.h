#ifndef SContext_H_INCLUDED
#define SContext_H_INCLUDED


#define DEBUG_INFO
//#define DEBUG_PLAYER
#define DEBUG_OBJS
//#define DEBUG_GRID
//#define DEBUG_EFFECTS

#define DESKTOP_WND_X 854
#define DESKTOP_WND_Y 480

#include <irrTypes.h>

class android_app;

namespace irr
{
    class IrrlichtDevice;
}

class Object;
class ObjectManager;
class EffectRenderer;
struct SMaterials;


struct SContext
{
    android_app* App;
    irr::IrrlichtDevice* Device;

    Object* State;
    ObjectManager* ObjManager;

    EffectRenderer* Renderer;
    SMaterials* Mtls;

    irr::f32 GUIScale;
};

#endif // SContext_H_INCLUDED
