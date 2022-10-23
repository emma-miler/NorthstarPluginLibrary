#include "loader.h"
#include "plugin.h"

#include "squirrel.h"
#include <iostream>

#include "../include/spdlog/sinks/base_sink.h"
#include "logging.h"

void PLUGIN_INIT(PluginInitFuncs* funcs, PluginNorthstarData* data) {
	g_pPlugin = new Plugin;
	g_pPlugin->logger = (logger_t)funcs->logger;
	spdlog::default_logger()->sinks().pop_back();
	spdlog::default_logger()->sinks().push_back(std::make_shared<my_sink>());
	spdlog::info("PluginLibrary-plugin succesfully initialised!");

	g_pPlugin->Init(funcs, data);
}

void PLUGIN_INIT_SQVM_CLIENT(SquirrelFunctions* funcs) {
	InitializeSquirrelVM_CLIENT(funcs);
}

void PLUGIN_INIT_SQVM_SERVER(SquirrelFunctions* funcs) {
	InitializeSquirrelVM_SERVER(funcs);
}

void PLUGIN_INFORM_SQVM_CREATED(ScriptContext context, CSquirrelVM* sqvm) {
	switch (context)
	{
	case ScriptContext::CLIENT:
		g_pSquirrel<ScriptContext::CLIENT>->VMCreated(sqvm);
	case ScriptContext::SERVER:
		g_pSquirrel<ScriptContext::SERVER>->VMCreated(sqvm);
	case ScriptContext::UI:
		g_pSquirrel<ScriptContext::UI>->VMCreated(sqvm);
	default:
		spdlog::warn("PLUGIN_INFORM_SQVM_CREATED called with unknown ScriptContext {}", context);
	}
}

void PLUGIN_INFORM_SQVM_DESTROYED(ScriptContext context) {
	switch (context)
	{
	case ScriptContext::CLIENT:
		g_pSquirrel<ScriptContext::CLIENT>->VMDestroyed();
	case ScriptContext::SERVER:
		g_pSquirrel<ScriptContext::SERVER>->VMDestroyed();
	case ScriptContext::UI:
		g_pSquirrel<ScriptContext::UI>->VMDestroyed();
	default:
		spdlog::warn("PLUGIN_INFORM_SQVM_DESTROYED called with unknown ScriptContext {}", context);
	}
}

// TODO: this stuff should be saved to global structs
// since this is async, these should have shared_mutex's

void PLUGIN_RESPOND_SERVER_DATA(PluginServerData* data) {}
void PLUGIN_RESPOND_GAMESTATE_DATA(PluginGameStateData* data) {
	spdlog::info("Got gamestate data back from NS");
}
void PLUGIN_RESPOND_RPC_DATA(PluginServerData* serverdata, PluginGameStateData* gamestatedataa) {}