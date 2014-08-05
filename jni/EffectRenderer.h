#ifndef OBJECTEFFECTRENDERER_H
#define OBJECTEFFECTRENDERER_H

#include "SContext.h"
#include <irrlicht.h>
#include <irrPP.h>
#include "EffectFader.h"
#include "SMaterials.h"

#ifdef _IRR_ANDROID_PLATFORM_
#include <android_native_app_glue.h>
#endif // _IRR_ANDROID_PLATFORM_


using namespace irr;

enum E_EFFECT_TYPE
{
    EET_FXAA = 0,
    EET_GLOW,
    EET_FADER
};


class EffectRenderer
{
public:
    EffectRenderer(SContext* cont);
    ~EffectRenderer();
    void drawAll(u32 timeDelta);

    // is at least one effect active?
    bool isActive() const;

    void init(E_EFFECT_TYPE type);

    void loadPP(bool reload = false);

    void setForceFXAAOff(bool force);

    core::dimension2du getScreenSize() const;

    EffectFader* getFader() const;

    video::irrPP* PP;

private:
    static const video::E_POSTPROCESSING_EFFECT_QUALITY EffectQuality = video::EPQ_QUARTER;

    SContext* Context;
    bool Active;
    core::dimension2du ScreenSize;
    bool GUIHasEffects;

    scene::ISceneManager* Smgr;
    scene::ICameraSceneNode* Camera;
    video::ITexture* Scene;

    bool ForceFXAAOff;
    video::CPostProcessingEffect* FXAA;
    video::CPostProcessingEffectChain* Glow;

    EffectFader* Fader;
};

#endif // OBJECTEFFECTRENDERER_H
