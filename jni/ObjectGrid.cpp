#include "ObjectGrid.h"

ObjectGrid::ObjectGrid(SContext* cont) : Object(cont),
    UpdateState(EGUS_NONE),
    CollisionActive(true),
    Generator(Context->Sets->Seed, NumPointsX, NumPointsZ),
    GenChangeIn(GenChangeEvery),
    SlopeChangeIn(SlopeChangeEvery),
    ColorChangeIn(5),
    ChangingColor(0)
{
    Name = "ObjectGrid";
    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_SPAWNED));

    Position = core::vector3df(0);

    for (u32 z = 0; z < NumPointsZ; z++)
        for (u32 x = 0; x < NumPointsX; x++)
            Points[z][x] = 0;

    srand(0);
    Generator.setType(EGT_PLAINS);

    CinLines = new ObjectGridCinematicLines(Context, NumPointsX);


    // starting color
    video::SColorf startCol = hueShift(video::SColorf(1, 1, 0), Generator.getRandomVal(Position.Z) * 180);
    ColorNear = startCol;
    ColorFar = startCol;

    Points[10][15] = 2;
    Points[11][15] = 1;
    Points[11][14] = 0.4;
    Points[12][15] = 1;

    Buffer = new scene::SMeshBuffer();
    Buffer->Vertices.set_used(NumPointsX * NumPointsZ * 6);
    Buffer->Indices.set_used(NumPointsX * NumPointsZ * 24);

    scene::SMesh* mesh = new scene::SMesh();
    mesh->setHardwareMappingHint(scene::EHM_STATIC);
    mesh->addMeshBuffer(Buffer);
    Buffer->drop();
    Node = Context->Device->getSceneManager()->addMeshSceneNode(mesh);
    Node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    Node->setMaterialType(Context->Mtls->Solid);
    Node->setAutomaticCulling(scene::EAC_OFF);
    mesh->drop();

    BufferAppx = new scene::SMeshBuffer();
    BufferAppx->Vertices.set_used(NumPointsX * NumPointsZ);
    BufferAppx->Indices.set_used((NumPointsX-1) * (NumPointsZ-1) * 6);

    scene::SMesh* backMesh = new scene::SMesh();
    backMesh->setHardwareMappingHint(scene::EHM_STATIC);
    backMesh->addMeshBuffer(BufferAppx);
    BufferAppx->drop();
    BackNode = Context->Device->getSceneManager()->addMeshSceneNode(backMesh);
    BackNode->setMaterialType(Context->Mtls->Solid);
    BackNode->getMaterial(0).AmbientColor = video::SColor(255, 0, 0, 0);
    BackNode->setAutomaticCulling(scene::EAC_OFF);
    backMesh->drop();

    regenerate();
    regenerateAppx();
}

ObjectGrid::~ObjectGrid()
{
    delete CinLines;

    Node->remove();
    BackNode->remove();

    Object* pl = Context->ObjManager->getObjectFromName("ObjectPlayer");
    if (pl)
        pl->unregisterObserver(this);

    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_DIED));
}

void ObjectGrid::onMessage(SMessage msg)
{
    if (msg.Type == EMT_OBJ_SPAWNED)
    {
        if (msg.Dispatcher->getName() == "ObjectPlayer")
            msg.Dispatcher->registerObserver(this);
    }
    else if (msg.Type == EMT_OBJ_POS)
    {
        core::vector3df pPos(msg.Position.X, 0, msg.Position.Z);
        core::vector3df diffVect = pPos - Position;

        if (handleCollision(core::vector3df(msg.Position.X, msg.Position.Y, msg.Position.Z), diffVect))
        {
            Context->ObjManager->broadcastMessage(SMessage(this, EMT_PLAYER_CRASHED));
            return;
        }

        switch (UpdateState)
        {
        case EGUS_NONE:
            break;

        case EGUS_LINES:
            CinLines->spawn(Position, Context->Mtls->GridCB->getNearColor(), Context->Mtls->GridCB->getFarColor(), Points[0], Points[1]);
            UpdateState = EGUS_ZDATA;
            return;

        case EGUS_ZDATA:
            addZ();
            handleGenUpdate();
            handleColors();
            UpdateState = EGUS_GRID;
            return;

        case EGUS_GRID:
            regenerate();
            UpdateState = EGUS_BACKGRID;
            return;

        case EGUS_BACKGRID:
            regenerateAppx();
            broadcastMessage(SMessage(this, EMT_GRID_REGENED));
            UpdateState = EGUS_NONE;
            return;
        }

        if (diffVect.getLength() > 1.0)
        {
            if (diffVect.X > 0.5)
            {
                addPlusX();
                UpdateState = EGUS_GRID;
                return;
            }
            else if (diffVect.X < -0.5)
            {
                addMinusX();
                UpdateState = EGUS_GRID;
                return;
            }

            if (diffVect.Z > 0.5)
            {
                UpdateState = EGUS_LINES;
            }
        }
    }
    else if (msg.Type == EMT_SERIALIZE)
    {
        Json::Value gridRoot;

        storeVector3df(Position, gridRoot, "pos");
        gridRoot["gen_changeIn"] = GenChangeIn;
        gridRoot["generator"] = Generator.serialize();

        gridRoot["color_changeIn"] = ColorChangeIn;
        gridRoot["color_changeing"] = ChangingColor;
        gridRoot["color_far"] = serializeSColorf(ColorFar);
        gridRoot["color_far_t"] = serializeSColorf(ColorFarTarget);
        gridRoot["color_far_o"] = serializeSColorf(ColorFarOld);
        gridRoot["color_near"] = serializeSColorf(ColorNear);
        gridRoot["color_near_o"] = serializeSColorf(ColorNearOld);

        for (u32 z = 0; z < NumPointsZ; z++)
        {
            Json::Value pointsZ;

            for (u32 x = 0; x < NumPointsX; x++)
            {
                pointsZ.append(Points[z][x]);
            }

            gridRoot["points"].append(pointsZ);
        }

        (*msg.SData.Root)[Name.c_str()] = gridRoot;
    }
    else if (msg.Type == EMT_DESERIALIZE)
    {
        Json::Value gridRoot = (*msg.SData.Root)[Name.c_str()];

        Position = parseVector3df(gridRoot, "pos");
        GenChangeIn = gridRoot["gen_changeIn"].asUInt();
        Generator.deserialize(gridRoot["generator"]);

        ColorChangeIn = gridRoot["color_changeIn"].asDouble();
        ChangingColor = gridRoot["color_changeing"].asDouble();
        ColorFar = deserializeSColorf(gridRoot["color_far"]);
        ColorFarTarget = deserializeSColorf(gridRoot["color_far_t"]);
        ColorFarOld = deserializeSColorf(gridRoot["color_far_o"]);
        ColorNear = deserializeSColorf(gridRoot["color_near"]);
        ColorNearOld = deserializeSColorf(gridRoot["color_near_o"]);

        for (u32 z = 0; z < NumPointsZ; z++)
        {
            for (u32 x = 0; x < NumPointsX; x++)
            {
                Points[z][x] = gridRoot["points"][z][x].asDouble();
            }
        }

        UpdateState = EGUS_GRID;
    }
}

void ObjectGrid::setColorMode(E_GRID_COLORMODE mode)
{
    Colormode = mode;
}

void ObjectGrid::setCollision(bool active)
{
    CollisionActive = active;

    if (CollisionActive)
        BackNode->setMaterialType(Context->Mtls->Solid);
    else
        BackNode->setMaterialType(Context->Mtls->Alpha);
}

bool ObjectGrid::getCollision() const
{
    return CollisionActive;
}

core::vector3df ObjectGrid::getPosition() const
{
    return Position;
}

u32 ObjectGrid::getNumPointsX() const
{
    return NumPointsX;
}

u32 ObjectGrid::getNumPointsZ() const
{
    return NumPointsZ;
}

f32 ObjectGrid::getBaseHeight(u32 z) const
{
    return Generator.getHeight(z);
}

f32 ObjectGrid::getHillHeight(u32 x, u32 z) const
{
    return Points[z][x] - getBaseHeight(z);
}

const GridGenerator& ObjectGrid::getGenerator() const
{
    return Generator;
}


void ObjectGrid::regenerate()
{
    u32 vertC = 0, indC = 0;

    const core::vector2df null2d(0);

    const core::vector3df center(NumPointsX/2.0, 0, OffsetZ);

    core::vector3df pointVec(0, Points[0][0], 0);

    for (u32 z = 0; z < NumPointsZ; z++)
    {
        pointVec.Z = z - center.Z;
        const video::SColor color = getGridColor(z).toSColor();

        for (u32 x = 0; x < NumPointsX; x++)
        {
            pointVec.X = x - center.X;
            pointVec.Y = Points[z][x];

            //const u32
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(1, 0, 0), color, null2d));
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(-1, 0, 0), color, null2d));
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(0, 1, 0), color, null2d));
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(0, -1, 0), color, null2d));
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(0, 0, 1), color, null2d));
            Buffer->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(0, 0, -1), color, null2d));

            if (x > 0)
            {
                //Y quad
                Buffer->Indices[indC++] = (vertC-4);
                Buffer->Indices[indC++] = (vertC-3);
                Buffer->Indices[indC++] = (vertC-10);
                Buffer->Indices[indC++] = (vertC-3);
                Buffer->Indices[indC++] = (vertC-9);
                Buffer->Indices[indC++] = (vertC-10);
                //Z quad
                Buffer->Indices[indC++] = (vertC-2);
                Buffer->Indices[indC++] = (vertC-1);
                Buffer->Indices[indC++] = (vertC-8);
                Buffer->Indices[indC++] = (vertC-1);
                Buffer->Indices[indC++] = (vertC-7);
                Buffer->Indices[indC++] = (vertC-8);
            }
            if (z > 0)
            {
                const u32 prevYVertC = ((z - 1) * NumPointsX + x) * 6;
                //X quad
                Buffer->Indices[indC++] = (vertC-5);
                Buffer->Indices[indC++] = (vertC-6);
                Buffer->Indices[indC++] = (prevYVertC+1);
                Buffer->Indices[indC++] = (vertC-6);
                Buffer->Indices[indC++] = (prevYVertC);
                Buffer->Indices[indC++] = (prevYVertC+1);
                //Y quad
                Buffer->Indices[indC++] = (vertC-4);
                Buffer->Indices[indC++] = (vertC-3);
                Buffer->Indices[indC++] = (prevYVertC+2);
                Buffer->Indices[indC++] = (vertC-3);
                Buffer->Indices[indC++] = (prevYVertC+3);
                Buffer->Indices[indC++] = (prevYVertC+2);
            }
        }
    }

    Buffer->Vertices.set_used(vertC);
    Buffer->Indices.set_used(indC);
    Buffer->setDirty();
    Node->setPosition(Position);
}

void ObjectGrid::regenerateAppx()
{
    u32 vertC = 0, indC = 0;

    const core::vector2df null2d(0);

    const core::vector3df center(NumPointsX/2.0, 0, OffsetZ);

    core::vector3df pointVec(0, Points[0][0], 0);

    for (u32 z = 0; z < NumPointsZ; z++)
    {
        pointVec.Z = z - center.Z;
        const video::SColor color = getBackColor(z).toSColor();

        for (u32 x = 0; x < NumPointsX; x++)
        {
            pointVec.X = x - center.X;
            pointVec.Y = Points[z][x];

            BufferAppx->Vertices[vertC++] = (video::S3DVertex(pointVec, core::vector3df(0, -1, 0), color, null2d));

            if (x > 0 && z > 0)
            {
                const u32 vertC = z * NumPointsX + x;
                const u32 prevYVertC = vertC - NumPointsX;

                if (Points[z][x] + Points[z-1][x-1] > Points[z-1][x] + Points[z][x-1])
                {
                    BufferAppx->Indices[indC++] = (vertC);
                    BufferAppx->Indices[indC++] = (prevYVertC);
                    BufferAppx->Indices[indC++] = (vertC-1);
                    BufferAppx->Indices[indC++] = (prevYVertC-1);
                    BufferAppx->Indices[indC++] = (vertC-1);
                    BufferAppx->Indices[indC++] = (prevYVertC);
                }
                else
                {
                    BufferAppx->Indices[indC++] = (vertC);
                    BufferAppx->Indices[indC++] = (prevYVertC);
                    BufferAppx->Indices[indC++] = (prevYVertC-1);
                    BufferAppx->Indices[indC++] = (prevYVertC-1);
                    BufferAppx->Indices[indC++] = (vertC-1);
                    BufferAppx->Indices[indC++] = (vertC);
                }
            }
        }
    }

    BufferAppx->Vertices.set_used(vertC);
    BufferAppx->Indices.set_used(indC);
    BufferAppx->setDirty();
    BackNode->setPosition(Position);
}

void ObjectGrid::addZ()
{
    memmove(Points, Points[1], sizeof(f32) * (NumPointsZ-1) * NumPointsX);
    memcpy(Points[NumPointsZ-1], Generator.generate(Position, EGD_FRONT), sizeof(f32) * NumPointsX);

    Position += core::vector3df(0, 0, 1);
}

void ObjectGrid::addPlusX()
{
    Generator.generate(Position, EGD_RIGHT);

    for (u32 z = 0; z < NumPointsZ; z++)
    {
        memmove(Points[z], &Points[z][1], sizeof(f32) * (NumPointsX - 1));
        Points[z][NumPointsX-1] = Generator.getGenerated(z);
    }

    Position += core::vector3df(1, 0, 0);
}

void ObjectGrid::addMinusX()
{
    Generator.generate(Position, EGD_LEFT);

    for (u32 z = 0; z < NumPointsZ; z++)
    {
        memmove(&Points[z][1], Points[z], sizeof(f32) * (NumPointsX - 1));
        Points[z][0] = Generator.getGenerated(z);
    }

    Position += core::vector3df(-1, 0, 0);
}

void ObjectGrid::handleGenUpdate()
{
    Generator.setDifficulty(Generator.getDifficulty() + 0.5 / GenChangeEvery);

    GenChangeIn--;
    SlopeChangeIn--;

    if (GenChangeIn <= 0)
    {
        f32 randVal = (Generator.getRandomVal(Position.Z) + 1) / 2 - 0.00001;
        u32 genChoice = u32(randVal * f32(EGT_COUNT));

        Generator.setType(E_GEN_TYPE(genChoice));
        Generator.setDifficulty(Generator.getDifficulty() - 0.45);

        GenChangeIn = GenChangeEvery;
    }

    if (SlopeChangeIn <= 0)
    {
        f32 randVal = (Generator.getRandomVal(Position.Z) + 1) / 2 - 0.00001;
        u32 slopeChoice = u32(randVal * (EST_COUNT + NoSlopeWeight - 1));
        u32 changeInMod = 0;
        if (slopeChoice >= EST_COUNT - 1)
        {
            slopeChoice = EST_NONE;
        }
        else if (slopeChoice == 0)
        {
            slopeChoice = EST_UP;
            if (Generator.getSlope() == EST_DOWN)
            {
                changeInMod = Generator.getSlopeChangeInSteps();
            }
        }
        else
        {
            slopeChoice = EST_DOWN;
            if (Generator.getSlope() == EST_UP)
            {
                changeInMod = Generator.getSlopeChangeInSteps();
            }
        }

        Generator.setSlope(E_SLOPE_TYPE(slopeChoice));

        SlopeChangeIn = SlopeChangeEvery + changeInMod + u32(randVal * SlopeChangeEveryOffset);
    }
}

void ObjectGrid::handleColors()
{
    ColorChangeIn--;
    if (ColorChangeIn == 0)
    {
        ColorNearOld = ColorNear;
        ColorFarOld = ColorFar;
        ColorFarTarget = hueShift(ColorFar, 120);

        ChangingColor = NumPointsZ;
        ColorChangeIn = ColorChangeEvery;

#ifdef DEBUG_GRID
        debugLog("Starting color change");
#endif // DEBUG_GRID
    }
    if (ChangingColor > 0)
    {
        ColorNear = ColorFarOld.getInterpolated(ColorNearOld, (NumPointsZ - ChangingColor) / f32(NumPointsZ));
        ColorFar = ColorFarTarget.getInterpolated(ColorFarOld, (NumPointsZ - ChangingColor) / f32(NumPointsZ));
        ChangingColor--;

        /*if (ChangingColor == 0 && near.toSColor() != far.toSColor())
        {
            ColorFar = ColorNext;
            ChangingColor = NumPointsZ;
        }*/
    }
}

const inline video::SColorf ObjectGrid::getGridColor(u32 z)
{
    switch(Colormode)
    {
        case EGC_CLASSIC:
            return ColorFar.getInterpolated(ColorNear, f32(z) / NumPointsZ);
        default:
            return video::SColorf();
    }
}

const inline video::SColorf ObjectGrid::getBackColor(u32 z)
{
    switch(Colormode)
    {
        case EGC_CLASSIC:
            return video::SColorf(0, 0, 0, 1);
        default:
            return video::SColorf();
    }
}

bool ObjectGrid::handleCollision(core::vector3df pPos, core::vector3df diffV)
{
    SMessage msg(this, EMT_PLAYER_FEEDBACK);

    msg.PlayerFeedback.GridAngle = -radToDeg(atan(getBaseHeight(OffsetZ+1) - getBaseHeight(OffsetZ)));
    msg.PlayerFeedback.Height = getBaseHeight(OffsetZ) + (getBaseHeight(OffsetZ+1) - getBaseHeight(OffsetZ)) * (pPos.Z - Position.Z);

    // "hack"
    Context->ObjManager->getObjectFromName("ObjectPlayer")->onMessage(msg);


    if (!CollisionActive)
        return false;


    u32 halfPtsX = NumPointsX / 2;
    u32 posXOffset = diffV.X < 0 ? 1 : 0;
    u32 posZOffset = 1;//diffV.Z > 0.5 ? 1 : 0;
    f32 posX = Position.X - posXOffset;
    f32 posZ = Position.Z;

    core::vector3df ld(posX, 0, posZ);
    ld.Y = Points[posZOffset][halfPtsX - posXOffset];

    core::vector3df rd(posX + 1, 0, posZ);
    rd.Y = Points[posZOffset][halfPtsX - posXOffset + 1];

    core::vector3df lu(posX, 0, posZ + 1);
    lu.Y = Points[posZOffset + 1][halfPtsX - posXOffset];

    core::vector3df ru(posX + 1, 0, posZ + 1);
    ru.Y = Points[posZOffset + 1][halfPtsX - posXOffset + 1];


    core::triangle3df t1, t2;

    if (ld.Y + ru.Y <= rd.Y + lu.Y)
    {
        t1.pointA = rd;
        t1.pointB = ld;
        t1.pointC = ru;
        t2.pointA = ld;
        t2.pointB = lu;
        t2.pointC = ru;
    }
    else
    {
        t1.pointA = rd;
        t1.pointB = ld;
        t1.pointC = lu;
        t2.pointA = rd;
        t2.pointB = lu;
        t2.pointC = ru;
    }

    core::vector3df t1n = t1.getNormal().normalize();
    core::vector3df t2n = t2.getNormal().normalize();
    core::line3df t1l(pPos + t1n, pPos - 999 * t1n);
    core::line3df t2l(pPos + t2n, pPos - 999 * t2n);

#ifdef DEBUG_PLAYER
    Context->Device->getVideoDriver()->setMaterial(video::SMaterial());
    Context->Device->getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);
    Context->Device->getVideoDriver()->draw3DTriangle(t1);
    Context->Device->getVideoDriver()->draw3DTriangle(t2);
#endif // DEBUG_PLAYER

    core::vector3df t1Int, t2Int;
    if (t1.getIntersectionWithLimitedLine(t1l, t1Int))
    {
        if ((pPos - t1Int).getLength() < PlayerSize)
        {
            return true;
        }

#ifdef DEBUG_PLAYER
        Context->Device->getVideoDriver()->draw3DLine(pPos, t1Int);
#endif // DEBUG_PLAYER
    }
    if (t2.getIntersectionWithLimitedLine(t2l, t2Int))
    {
        if ((pPos - t2Int).getLength() < PlayerSize)
        {
            return true;
        }

#ifdef DEBUG_PLAYER
        Context->Device->getVideoDriver()->draw3DLine(pPos, t2Int);
#endif // DEBUG_PLAYER
    }

    return false;
}
