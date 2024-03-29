#ifndef OBJECTGRID_H
#define OBJECTGRID_H

#include "ObjectManager.h"
#include "Settings.h"
#include "GridGenerator.h"
#include "ObjectGridCinematicLines.h"
#include "SMaterials.h"
#include "ShaderCBGrid.h"
#include "ShaderCBAlpha.h"
#include "ShaderCBSolid.h"
#include "functions.h"

#include <irrlicht.h>
#include <vector>
#include "libnoise/perlin.h"

using namespace irr;

enum E_GRID_UPDATE_STATE
{
    EGUS_LINES = 0,
    EGUS_ZDATA,
    EGUS_GRID,
    EGUS_BACKGRID,
    EGUS_NONE
};

enum E_GRID_COLORMODE
{
    EGC_CLASSIC = 0
};

class ObjectGrid : public Object
{
public:
    ObjectGrid(SContext* cont);
    ~ObjectGrid();
    virtual void onMessage(SMessage msg);

    void setColorMode(E_GRID_COLORMODE mode);

    void setCollision(bool active);
    bool getCollision() const;

    core::vector3df getPosition() const;
    u32 getNumPointsZ() const;
    u32 getNumPointsX() const;

    f32 getBaseHeight(u32 z) const;
    f32 getHillHeight(u32 x, u32 z) const;

    const GridGenerator& getGenerator() const;


private:
    static constexpr u32 NumPointsZ = 18;
    static constexpr u32 NumPointsX = NumPointsZ * 2;
    static constexpr u32 OffsetZ = 1; // how much is the mesh offseted from it's origin
    static constexpr f32 LineThickness = 0.005;
    static constexpr u32 GenChangeEvery = 50;
    static constexpr u32 SlopeChangeEvery = 7;
    static constexpr u32 SlopeChangeEveryOffset = 0; // random offset for slope change
    static constexpr u32 NoSlopeWeight = 2; // probability of no-slope vs. slope
    static constexpr u32 ColorChangeEvery = 45;
    static constexpr f32 PlayerSize = 0.05; // needed for collision handling

    E_GRID_COLORMODE Colormode = EGC_CLASSIC;
    core::vector3df Position;
    E_GRID_UPDATE_STATE UpdateState;
    f32 Points[NumPointsZ][NumPointsX];

    bool CollisionActive;

    GridGenerator Generator;
    u32 GenChangeIn;
    u32 SlopeChangeIn;

    ObjectGridCinematicLines* CinLines;

    u32 ColorChangeIn;
    u32 ChangingColor;
    video::SColorf ColorNear, ColorNearOld, ColorFar, ColorFarTarget, ColorFarOld;

    scene::IMeshSceneNode *Node, *BackNode;
    //std::vector<>
    scene::SMeshBuffer *Buffer, *BufferAppx;

    void regenerate();
    void regenerateAppx();
    void addZ();
    void addPlusX();
    void addMinusX();

    void handleGenUpdate();

    void handleColors();
    const inline video::SColorf getGridColor(u32 z);
    const inline video::SColorf getBackColor(u32 z);

    bool handleCollision(core::vector3df pPos, core::vector3df diffV);
};

#endif // OBJECTGRID_H
