#ifndef OBJECTGRIDCINEMATICLINES_H
#define OBJECTGRIDCINEMATICLINES_H

#include "Object.h"
#include "ObjectManager.h"
#include "GeometryGenerator.h"
#include "SMaterials.h"
#include <irrlicht.h>
#include <vector>
#include <mutex>
#include <atomic>
#include <stdlib.h>

using namespace irr;


class ObjectGridCinematicLines : public Object
{
public:
    ObjectGridCinematicLines(SContext* cont, u32 numPtsX);
    virtual ~ObjectGridCinematicLines();

    virtual void onMessage(SMessage msg);

    void spawn(core::vector3df pos, video::SColorf col, video::SColorf farcol, f32* last, f32* prev);

    u32 getLineCount() const;

    u32 getGroupCount() const;

private:
    struct LineGroup
    {
        struct Line
        {
            core::vector3df dirV;
            f32 rotV;
            f32 ttl;
            bool dead;

            Line(core::vector3df d, f32 r, f32 t)
                :dirV(d), rotV(r), ttl(t), dead(false)
            {

            }
        };

        std::vector<Line> Lines;
        scene::IMeshSceneNode* Node;
        bool Dead;
    };

    void updateLineGroup(LineGroup& group, f32 fDelta);
    void cleanDeadGroups();


    u32 NumPointsX;
    std::vector<LineGroup> LGroups;
    std::mutex UpdateMutex;
    std::atomic<bool> Dead;
    scene::IMesh* LineMeshZ;
    scene::IMesh* LineMeshX;


    static constexpr f32 ExistTime = 3;
    static constexpr f32 ExistTimeOffset = 2;
    static constexpr f32 FadeTime = 1;

    static constexpr f32 MinDirVelocity = 0.75;
    static constexpr f32 DirVelocityOffset = 0.5;
    static constexpr f32 MinRotVelocity = -2;
    static constexpr f32 RotVelocityOffset = 10;
};

#endif // OBJECTGRIDCINEMATICLINES_H
