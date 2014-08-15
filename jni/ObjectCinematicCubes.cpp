#include "ObjectCinematicCubes.h"

ObjectCinematicCubes::ObjectCinematicCubes(SContext* cont) : Object(cont)
{
    Name = "ObjectCinematicCubes";
    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_SPAWNED));

    Parent = Context->Device->getSceneManager()->addEmptySceneNode();

    Context->ObjManager->getObjectFromName("ObjectUpdater")->registerObserver(this);
}

ObjectCinematicCubes::~ObjectCinematicCubes()
{
    Context->ObjManager->getObjectFromName("ObjectUpdater")->unregisterObserver(this);
    Parent->remove();

    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_DIED));
}

void ObjectCinematicCubes::onMessage(SMessage msg)
{
    if (msg.Type == EMT_UPDATE)
    {
        if (Cubes.size() < MaxCubes)
        {
            scene::ISceneManager* smgr = Context->Device->getSceneManager();
            scene::IMeshSceneNode* cube = smgr->addMeshSceneNode(smgr->getMesh("cube-mesh-filled"), Parent);
            cube->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
            cube->setMaterialType(Context->Mtls->ItemCube);

            f32 depth = (SpawnDepth - SpawnDepthRandomOffset) + (rand() % u32(SpawnDepthRandomOffset * 2 * 100)) / 100.0;
            core::vector3df startP = getRandomPointOnCircle();
            core::vector3df endP = getRandomPointOnCircle();
            startP.Z = endP.Z = depth;
            scene::ISceneNodeAnimator* flyA = smgr->createFlyStraightAnimator(startP, endP, (TravelTime - TravelTimeRandomOffset) + rand() % (TravelTimeRandomOffset * 2));
            cube->addAnimator(flyA);
            scene::ISceneNodeAnimator* rotA = smgr->createRotationAnimator(core::vector3df(((rand() % 100) / 100.0) * MaxRotationSpeed));
            cube->addAnimator(rotA);

            Cubes.push_back(cube);
            flyA->drop();
            rotA->drop();
        }

        u32 numCubes = Cubes.size();
        for (u32 i = 0; i < numCubes; i++)
        {
            if ((*(Cubes[i]->getAnimators().begin()))->hasFinished())
            {
                Cubes[i]->remove();
                Cubes.erase(i);
                break;
            }
        }
    }
}

void ObjectCinematicCubes::setVisible(bool vis)
{
    Parent->setVisible(vis);
}

bool ObjectCinematicCubes::isVisible() const
{
    return Parent->isVisible();
}

core::vector3df ObjectCinematicCubes::getRandomPointOnCircle()
{
    u32 randN = rand() % 360;
    core::vector3df point(cos(randN), sin(randN), 0);
    point.setLength(SpawnCircleRadius);

    return point;
}
