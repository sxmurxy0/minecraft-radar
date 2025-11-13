#pragma once

#include <jni.h>
#include <string>

static const std::string
	MINECRAFT_CLASS = "flk", // net.minecraft.client.Minecraft
	MINECRAFT_INSTANCE_FIELD = "F", // instance
	MINECRAFT_INSTANCE_FIELD_S = "Lflk;", // Lnet.minecraft.client.Minecraft;

	LOCALPLAYER_CLASS = "gkx", //  net.minecraft.client.player.LocalPlayer
	MINECRAFT_PLAYER_FIELD = "t", // player
	MINECRAFT_PLAYER_FIELD_S = "Lgkx;", // Lnet.minecraft.client.player.LocalPlayer;

	CLIENTLEVEL_CLASS = "gga", // net.minecraft.client.multiplayer.ClientLevel
	MINECRAFT_LEVEL_FIELD = "s", // level
	MINECRAFT_LEVEL_FIELD_S = "Lgga;", // Lnet.minecraft.client.multiplayer.ClientLevel;

	CLIENTLEVEL_PLAYERS_FIELD = "L", // players
	CLIENTLEVEL_PLAYERS_FIELD_S = "Ljava/util/List;", // java List

	ENTITY_CLASS = "bum", // net.minecraft.world.entity.Entity
	ENTITY_X_FIELD = "aa", // xOld
	ENTITY_X_FIELD_S = "D", // double
	ENTITY_Z_FIELD = "ac", // yOld
	ENTITY_Z_FIELD_S = "D"; // double

class PlayerProvider {
private:
	jclass minecraftClass = nullptr;
	jobject minecraftObject = nullptr;

public:
	void Init();

	jobject GetLocalPlayerObject();
	jobject GetPlayerListObject();
	std::pair<jdouble, jdouble> GetEntityPosition(jobject entity);
};

extern PlayerProvider g_PlayerProvider;