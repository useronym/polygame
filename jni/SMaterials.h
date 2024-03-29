#ifndef SMATERIALS_H_INCLUDED
#define SMATERIALS_H_INCLUDED

#include <irrlicht.h>
#include "ShaderCBAlpha.h"
#include "ShaderCBFader.h"
#include "ShaderCBGrid.h"
#include "ShaderCBSky.h"
#include "ShaderCBSolid.h"
#include "ShaderCBTracer.h"

using namespace irr;



struct SMaterials
{
    video::E_MATERIAL_TYPE Grid;
    ShaderCBGrid* GridCB;

    video::E_MATERIAL_TYPE Alpha;
    ShaderCBAlpha* AlphaCB;

    video::E_MATERIAL_TYPE Solid;
    ShaderCBSolid* SolidCB;

    video::E_MATERIAL_TYPE Fader;
    ShaderCBFader* FaderCB;

    video::E_MATERIAL_TYPE Sky;

    video::E_MATERIAL_TYPE Tracer;
    ShaderCBTracer* TracerCB;

    bool Loaded;

    void setTransform(f32 t)
    {
        if (Loaded)
        {
            GridCB->setTransform(t);
            AlphaCB->setTransform(t);
            SolidCB->setTransform(t);
            TracerCB->setTransform(t);
        }
    }

    f32 getTransform() const
    {
        if (Loaded)
            return GridCB->getTransform();

        return 0;
    }
};


#endif // SMATERIALS_H_INCLUDED
