#include "plugin.h"
#include "plugin_abi.h"

#include "../pch.h"

Plugin* g_pPlugin;

void Plugin::Init(PluginInitFuncs* funcs, PluginNorthstarData* data) {
	g_pPlugin->_RequestServerData = (PLUGIN_REQUESTS_SERVER_DATA_TYPE)GetProcAddress(data->northstarModule, "PLUGIN_REQUESTS_SERVER_DATA");
	g_pPlugin->_RequestGameStateData = (PLUGIN_REQUESTS_GAMESTATE_DATA_TYPE)GetProcAddress(data->northstarModule, "PLUGIN_REQUESTS_GAMESTATE_DATA");
	g_pPlugin->_RequestRPCData = (PLUGIN_REQUESTS_RPC_DATA_TYPE)GetProcAddress(data->northstarModule, "PLUGIN_REQUESTS_RPC_DATA");
}

void Plugin::RequestServerData() {
	_RequestServerData((PLUGIN_RESPOND_SERVER_DATA_TYPE*)&PLUGIN_RESPOND_SERVER_DATA);
}
void Plugin::RequestGameStateData() {
	_RequestGameStateData((PLUGIN_RESPOND_GAMESTATE_DATA_TYPE*)&PLUGIN_RESPOND_GAMESTATE_DATA);
}
void Plugin::RequestRPCData() {
	_RequestRPCData((PLUGIN_RESPOND_RPC_DATA_TYPE*)&PLUGIN_RESPOND_RPC_DATA);
}