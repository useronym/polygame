#include "ObjectTracerRecorder.h"

ObjectTracerRecorder::ObjectTracerRecorder(SContext* cont) : Object(cont)
{
    Name = "ObjectTracerRecorder";
    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_SPAWNED));

    Positions.push_back(core::vector3df(0));

    Context->ObjManager->getObjectFromName("ObjectPlayer")->registerObserver(this);
}

ObjectTracerRecorder::~ObjectTracerRecorder()
{
    Object* pl = Context->ObjManager->getObjectFromName("ObjectPlayer");
    if (pl)
    {
        pl->unregisterObserver(this);
    }

    Context->ObjManager->broadcastMessage(SMessage(this, EMT_OBJ_DIED));
}

void ObjectTracerRecorder::onMessage(SMessage msg)
{
    if (msg.Type == EMT_OBJ_POS)
    {
        core::vector3df pPos(msg.Position.X, msg.Position.Y, msg.Position.Z);
        if ((pPos - Positions.back()).getLength() > RecordEvery)
        {
            Positions.push_back(pPos);
        }
    }
    else if (msg.Type == EMT_PLAYER_CRASHED)
    {
        writeToStorage();
    }
    else if (msg.Type == EMT_SERIALIZE)
    {
        Json::Value root;

        root["size"] = u32(Positions.size());

        for (auto& it : Positions)
        {
            root["positions"].append(storeVector3df(it));
        }

        (*msg.SData.Root)[Name.c_str()] = root;
    }
    else if (msg.Type == EMT_DESERIALIZE)
    {
        Json::Value root = (*msg.SData.Root)[Name.c_str()];

        u32 pSize = root["size"].asUInt();
        Positions.reserve(pSize);

        for (u32  i = 0; i < pSize; i++)
        {
            Positions.push_back(parseVector3df(root["positions"][i]));
        }
    }
}

void ObjectTracerRecorder::writeToStorage() const
{
    // tracers.json entry
    std::time_t t = time(0);
    struct std::tm* tNow = std::localtime(&t);
    Json::Value tracers = readJson(Context, Context->StoragePath + TRACERS_FILENAME);
    Json::Value entry;

    entry["seed"] = Context->Sets->Seed;
    entry["dist"] = Positions.back().Z;
    entry["count"] = u32(Positions.size());
    entry["date"] = serializeDate(tNow);
    core::stringc filename = core::stringc(t) + TRACER_EXT;
    entry["filename"] = filename.c_str();

    tracers.append(entry);
    writeJson(Context, tracers, Context->StoragePath + TRACERS_FILENAME);

    // binary Positions file
    io::IWriteFile* posF = Context->Device->getFileSystem()->createAndWriteFile(Context->StoragePath + filename);
    posF->write(Positions.data(), Positions.size() * sizeof(core::vector3df));
    posF->drop();
}
