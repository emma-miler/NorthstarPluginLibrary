#include "loader.h"

#include "lib/squirrel.h"
#include <iostream>

#include "include/spdlog/sinks/base_sink.h"

template<typename Mutex>
class sink : public spdlog::sinks::base_sink <Mutex>
{
public:

	void sink_it_(const spdlog::details::log_msg& in_msg) override
	{
		LogMsg msg{};
		std::string payload(in_msg.payload.data(), in_msg.payload.size());
		msg.level = (int) in_msg.level;
		msg.msg = payload.c_str();
		msg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(in_msg.time.time_since_epoch()).count();
		msg.source.file = in_msg.source.filename;
		msg.source.func = in_msg.source.funcname;
		msg.source.line = in_msg.source.line;
		g_pPlugin->logger(&msg);
	}

	void flush_() override
	{
		std::cout << std::flush;
	}

	protected:
		// sink log level - default is all
		spdlog::level_t level_{ spdlog::level::trace };
};

using my_sink = sink<std::mutex>;

void PLUGIN_INIT(PluginInitFuncs* funcs) {
	g_pPlugin = new Plugin{};
	g_pPlugin->logger = (logger_t) funcs->logger;
	std::cout << "plugin_init" << std::endl;
	spdlog::default_logger()->sinks().pop_back();
	spdlog::default_logger()->sinks().push_back(std::make_shared<my_sink>());
	spdlog::info("Testing");
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