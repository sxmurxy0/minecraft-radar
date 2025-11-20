#include "player_provider.h"

extern JNIEnv* g_JniEnv;

jclass PlayerProvider::minecraftClass = nullptr;

void PlayerProvider::Init() {
    minecraftClass = g_JniEnv->FindClass(MINECRAFT_CLASS.c_str());

    static jfieldID instanceFieldID = g_JniEnv->GetStaticFieldID(
        minecraftClass,
        MINECRAFT_INSTANCE_FIELD.c_str(),
        MINECRAFT_INSTANCE_FIELD_S.c_str()
    );
    minecraftObject = g_JniEnv->GetStaticObjectField(minecraftClass, instanceFieldID);
}

jobject PlayerProvider::GetLocalPlayerObject() {
    static jfieldID playerFieldID = g_JniEnv->GetFieldID(
        minecraftClass,
        MINECRAFT_PLAYER_FIELD.c_str(),
        MINECRAFT_PLAYER_FIELD_S.c_str()
    );

    return g_JniEnv->GetObjectField(minecraftObject, playerFieldID);
}

jobject PlayerProvider::GetPlayerListObject() {
    static jfieldID levelFieldID = g_JniEnv->GetFieldID(
        minecraftClass,
        MINECRAFT_LEVEL_FIELD.c_str(),
        MINECRAFT_LEVEL_FIELD_S.c_str()
    );

    jobject levelObject = g_JniEnv->GetObjectField(minecraftObject, levelFieldID);

    if (!levelObject) {
        return nullptr;
    }

    static jclass levelClass = g_JniEnv->FindClass(CLIENTLEVEL_CLASS.c_str());
    static jfieldID playersFieldID = g_JniEnv->GetFieldID(
        levelClass,
        CLIENTLEVEL_PLAYERS_FIELD.c_str(),
        CLIENTLEVEL_PLAYERS_FIELD_S.c_str()
    );

    return g_JniEnv->GetObjectField(levelObject, playersFieldID);
}

std::pair<jdouble, jdouble> PlayerProvider::GetEntityPosition(jobject entity) {
    static jclass entityClass = g_JniEnv->FindClass(ENTITY_CLASS.c_str());
    static jfieldID xEntityFieldID = g_JniEnv->GetFieldID(
        entityClass,
        ENTITY_X_FIELD.c_str(),
        ENTITY_X_FIELD_S.c_str()
    );
    static jfieldID zEntityFieldID = g_JniEnv->GetFieldID(
        entityClass,
        ENTITY_Z_FIELD.c_str(),
        ENTITY_Z_FIELD_S.c_str()
    );

    jdouble x = g_JniEnv->GetDoubleField(entity, xEntityFieldID);
    jdouble z = g_JniEnv->GetDoubleField(entity, zEntityFieldID);

    return {x, z};
}