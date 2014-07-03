#include "ObjectSky.h"

ObjectSky::ObjectSky(SContext* cont) : Object(cont)
{
    Name = "ObjectSky";
    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_SPAWNED));

    SkyNode = Context->Device->getSceneManager()->addSkyDomeSceneNode(Context->Device->getVideoDriver()->getTexture("sky.png"), 16, 18, 0.9, 2.0, 19);
    SkyNode->setMaterialType(Context->Mtls->Sky);
}

ObjectSky::~ObjectSky()
{
    //dtor
}

void ObjectSky::onMessage(SMessage msg)
{
    if (msg.Type == EMT_OBJ_POS)
    {
        SkyNode->setPosition(core::vector3df(msg.Position.X, msg.Position.Y, msg.Position.Z));
    }
    else if (msg.Type == EMT_OBJ_SPAWNED)
    {
        if (msg.Dispatcher->getName() == "ObjectPlayer")
            msg.Dispatcher->registerObserver(this);
    }
}