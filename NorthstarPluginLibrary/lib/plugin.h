#pragma once

#include "plugin_abi.h"
#include "loader.h"

typedef void (*logger_t)(void*);


class Plugin {
	public:
		logger_t logger;
		void Init(PluginInitFuncs* funcs, PluginNorthstarData* data);

	private:
		PLUGIN_REQUESTS_SERVER_DATA_TYPE _RequestServerData;
		PLUGIN_REQUESTS_GAMESTATE_DATA_TYPE _RequestGameStateData;
		PLUGIN_REQUESTS_RPC_DATA_TYPE _RequestRPCData;

	public:
		void RequestServerData();
		void RequestGameStateData();
		void RequestRPCData();
};

extern Plugin* g_pPlugin;