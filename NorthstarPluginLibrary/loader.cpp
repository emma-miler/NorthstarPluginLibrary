#include "loader.h"

#include "lib/squirrel.h"

void PLUGIN_INIT() {

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