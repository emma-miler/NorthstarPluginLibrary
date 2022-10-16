typedef void (*logger_t)(void*);

class Plugin {
	public:
		logger_t logger;
};

extern Plugin* g_pPlugin;