#pragma once

#include "squirrelclasstypes.h"
#include "vector.h"

const char* GetContextName(ScriptContext context);
eSQReturnType SQReturnTypeFromString(const char* pReturnType);
const char* SQTypeNameFromID(const int iTypeId);

template <ScriptContext context> class SquirrelManager
{
  private:
	std::vector<SQFuncRegistration*> m_funcRegistrations;

  public:
	CSquirrelVM* m_pSQVM;
	std::map<std::string, SQFunction> m_funcOverrides = {};
	std::map<std::string, SQFunction> m_funcOriginals = {};

	bool m_bFatalCompilationErrors = false;

#pragma region SQVM funcs
	RegisterSquirrelFuncType RegisterSquirrelFunc;
	sq_defconstType __sq_defconst;

	sq_compilebufferType __sq_compilebuffer;
	sq_callType __sq_call;
	sq_raiseerrorType __sq_raiseerror;

	sq_newarrayType __sq_newarray;
	sq_arrayappendType __sq_arrayappend;

	sq_newtableType __sq_newtable;
	sq_newslotType __sq_newslot;

	sq_pushroottableType __sq_pushroottable;
	sq_pushstringType __sq_pushstring;
	sq_pushintegerType __sq_pushinteger;
	sq_pushfloatType __sq_pushfloat;
	sq_pushboolType __sq_pushbool;
	sq_pushassetType __sq_pushasset;
	sq_pushvectorType __sq_pushvector;
	sq_pushSQObjectType __sq_pushSQObject;

	sq_getstringType __sq_getstring;
	sq_getintegerType __sq_getinteger;
	sq_getfloatType __sq_getfloat;
	sq_getboolType __sq_getbool;
	sq_getType __sq_get;
	sq_getassetType __sq_getasset;
	sq_getuserdataType __sq_getuserdata;
	sq_getvectorType __sq_getvector;

	sq_createuserdataType __sq_createuserdata;
	sq_setuserdatatypeidType __sq_setuserdatatypeid;

#pragma endregion

#pragma region MessageBuffer
	SquirrelMessageBuffer* messageBuffer;
	sq_getSquirrelFunctionType __sq_getSquirrelFunction;
	sq_schedule_call_externalType __sq_schedule_call_external;

	template <typename... Args> SquirrelMessage schedule_call(const char* funcname, Args... args)
	{
		// This function schedules a call to be executed on the next frame
		// This is useful for things like threads and plugins, which do not run on the main thread
		FunctionVector function_vector;
		SqRecurseArgs<context>(function_vector, args...);
		SquirrelMessage message = {funcname, function_vector};
		messageBuffer->push(message);
		return message;
	}

	SquirrelMessage schedule_call(const char* funcname)
	{
		// This function schedules a call to be executed on the next frame
		// This is useful for things like threads and plugins, which do not run on the main thread
		FunctionVector function_vector = {};
		SquirrelMessage message = {funcname, function_vector};
		messageBuffer->push(message);
		schedule_call_external(funcname, & unwind_message<context>);
		return message;
	}

	template <typename... Args> SQRESULT call(const char* funcname, Args... args)
	{
		// Warning!
		// This function assumes the squirrel VM is stopped/blocked at the moment of call
		// Calling this function while the VM is running is likely to result in a crash due to stack destruction
		// If you want to call into squirrel asynchronously, use `schedule_call` instead

		SQObject* functionobj = new SQObject();
		int result = g_pSquirrel<context>->sq_getSquirrelFunction(g_pSquirrel<context>->m_pSQVM->sqvm, funcname, functionobj, 0);
		if (result != 0) // This func returns 0 on success for some reason
		{
			return SQRESULT_ERROR;
		}
		g_pSquirrel<context>->pushSQObject(g_pSquirrel<context>->m_pSQVM->sqvm, functionobj); // Push the function object
		g_pSquirrel<context>->pushroottable(g_pSquirrel<context>->m_pSQVM->sqvm); // Push root table

		FunctionVector function_vector;
		SqRecurseArgs<context>(function_vector, args...);

		for (auto& v : function_vector)
		{
			// Execute lambda to push arg to stack
			v();
		}

		return g_pSquirrel<context>->call(g_pSquirrel<context>->m_pSQVM->sqvm, function_vector.size());
	}

#pragma endregion

  public:
	SquirrelManager() : m_pSQVM(nullptr) {}

	void VMCreated(CSquirrelVM* newSqvm);
	void VMDestroyed();
	void ExecuteCode(const char* code);
	void AddFuncRegistration(std::string returnType, std::string name, std::string argTypes, std::string helpText, SQFunction func);
	SQRESULT setupfunc(const SQChar* funcname);
	void AddFuncOverride(std::string name, SQFunction func);

#pragma region SQVM func wrappers
	inline void defconst(CSquirrelVM* sqvm, const SQChar* pName, int nValue)
	{
		__sq_defconst(sqvm, pName, nValue);
	}

	inline void schedule_call_external(const char* name, SquirrelMessage_External_Pop function)
	{
		__sq_schedule_call_external(context, name, function);
	}

	inline SQRESULT
	compilebuffer(CompileBufferState* bufferState, const SQChar* bufferName = "unnamedbuffer", const SQBool bShouldThrowError = false)
	{
		return __sq_compilebuffer(m_pSQVM->sqvm, bufferState, bufferName, -1, bShouldThrowError);
	}

	inline SQRESULT call(HSquirrelVM* sqvm, const SQInteger args)
	{
		return __sq_call(sqvm, args + 1, false, false);
	}

	inline SQInteger raiseerror(HSquirrelVM* sqvm, const const SQChar* sError)
	{
		return __sq_raiseerror(sqvm, sError);
	}

	inline void newarray(HSquirrelVM* sqvm, const SQInteger stackpos = 0)
	{
		__sq_newarray(sqvm, stackpos);
	}

	inline SQRESULT arrayappend(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_arrayappend(sqvm, stackpos);
	}

	inline SQRESULT newtable(HSquirrelVM* sqvm)
	{
		return __sq_newtable(sqvm);
	}

	inline SQRESULT newslot(HSquirrelVM* sqvm, SQInteger idx, SQBool bStatic)
	{
		return __sq_newslot(sqvm, idx, bStatic);
	}

	inline void pushroottable(HSquirrelVM* sqvm)
	{
		__sq_pushroottable(sqvm);
	}

	inline void pushstring(HSquirrelVM* sqvm, const SQChar* sVal, int length = -1)
	{
		__sq_pushstring(sqvm, sVal, length);
	}

	inline void pushinteger(HSquirrelVM* sqvm, const SQInteger iVal)
	{
		__sq_pushinteger(sqvm, iVal);
	}

	inline void pushfloat(HSquirrelVM* sqvm, const SQFloat flVal)
	{
		__sq_pushfloat(sqvm, flVal);
	}

	inline void pushbool(HSquirrelVM* sqvm, const SQBool bVal)
	{
		__sq_pushbool(sqvm, bVal);
	}

	inline void pushasset(HSquirrelVM* sqvm, const SQChar* sVal, int length = -1)
	{
		__sq_pushasset(sqvm, sVal, length);
	}

	inline void pushvector(HSquirrelVM* sqvm, const Vector3 pVal)
	{
		__sq_pushvector(sqvm, *(float**)&pVal);
	}

	inline void pushSQObject(HSquirrelVM* sqvm, SQObject* obj)
	{
		__sq_pushSQObject(sqvm, obj);
	}

	inline const SQChar* getstring(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_getstring(sqvm, stackpos);
	}

	inline SQInteger getinteger(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_getinteger(sqvm, stackpos);
	}

	inline SQFloat getfloat(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_getfloat(sqvm, stackpos);
	}

	inline SQBool getbool(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_getbool(sqvm, stackpos);
	}

	inline SQRESULT get(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		return __sq_get(sqvm, stackpos);
	}

	inline Vector3 getvector(HSquirrelVM* sqvm, const SQInteger stackpos)
	{
		float* pRet = __sq_getvector(sqvm, stackpos);
		return *(Vector3*)&pRet;
	}

	inline int sq_getSquirrelFunction(HSquirrelVM* sqvm, const char* name, SQObject* returnObj, const char* signature) {
		return __sq_getSquirrelFunction(sqvm, name, returnObj, signature);
	}


	inline SQRESULT getasset(HSquirrelVM* sqvm, const SQInteger stackpos, const char** result)
	{
		return __sq_getasset(sqvm, stackpos, result);
	}

	template <typename T> inline SQRESULT getuserdata(HSquirrelVM* sqvm, const SQInteger stackpos, T* data, uint64_t* typeId)
	{
		return __sq_getuserdata(sqvm, stackpos, (void**)data, typeId); // this sometimes crashes idk
	}

	template <typename T> inline T* createuserdata(HSquirrelVM* sqvm, SQInteger size)
	{
		void* ret = __sq_createuserdata(sqvm, size);
		memset(ret, 0, size);
		return (T*)ret;
	}

	inline SQRESULT setuserdatatypeid(HSquirrelVM* sqvm, const SQInteger stackpos, uint64_t typeId)
	{
		return __sq_setuserdatatypeid(sqvm, stackpos, typeId);
	}
#pragma endregion
};

template <ScriptContext context> SquirrelManager<context>* g_pSquirrel;

void InitializeSquirrelVM_CLIENT(SquirrelFunctions* funcs);
void InitializeSquirrelVM_SERVER(SquirrelFunctions* funcs);

template <ScriptContext context> void unwind_message(HSquirrelVM* sqvm);

/*
	Beware all ye who enter below.
	This place is not a place of honor... no highly esteemed deed is commemorated here... nothing valued is here.
	What is here was dangerous and repulsive to us. This message is a warning about danger.
*/

#pragma region MessageBuffer templates

// Clang-formatting makes this whole thing unreadable
// clang-format off

#ifndef MessageBufferFuncs
#define MessageBufferFuncs
// Bools
template <ScriptContext context, typename T>
requires std::convertible_to<T, bool> && (!std::is_floating_point_v<T>) && (!std::convertible_to<T, std::string>) && (!std::convertible_to<T, int>)
inline VoidFunction SQMessageBufferPushArg(T& arg) {
	return [arg]{ g_pSquirrel<context>->pushbool(g_pSquirrel<context>->m_pSQVM->sqvm, static_cast<bool>(arg)); };
}
// Vectors
template <ScriptContext context>
inline VoidFunction SQMessageBufferPushArg(Vector3& arg) {
	return [arg]{ g_pSquirrel<context>->pushvector(g_pSquirrel<context>->m_pSQVM->sqvm, arg); };
}
// Vectors
template <ScriptContext context>
inline VoidFunction SQMessageBufferPushArg(SQObject* arg) {
	return [arg]{ g_pSquirrel<context>->pushSQObject(g_pSquirrel<context>->m_pSQVM->sqvm, arg); };
}
// Ints
template <ScriptContext context, typename T>
requires std::convertible_to<T, int> && (!std::is_floating_point_v<T>)
inline VoidFunction SQMessageBufferPushArg(T& arg) {
	return [arg]{ g_pSquirrel<context>->pushinteger(g_pSquirrel<context>->m_pSQVM->sqvm, static_cast<int>(arg)); };
}
// Floats
template <ScriptContext context, typename T>
requires std::convertible_to<T, float> && (std::is_floating_point_v<T>)
inline VoidFunction SQMessageBufferPushArg(T& arg) {
	return [arg]{ g_pSquirrel<context>->pushfloat(g_pSquirrel<context>->m_pSQVM->sqvm, static_cast<float>(arg)); };
}
// Strings
template <ScriptContext context, typename T>
requires (std::convertible_to<T, std::string> || std::is_constructible_v<std::string, T>)
inline VoidFunction SQMessageBufferPushArg(T& arg) {
	auto converted = std::string(arg);
	return [converted]{ g_pSquirrel<context>->pushstring(g_pSquirrel<context>->m_pSQVM->sqvm, converted.c_str(), converted.length()); };
}
// Assets
template <ScriptContext context>
inline VoidFunction SQMessageBufferPushArg(SquirrelAsset& arg) {
	return [arg]{ g_pSquirrel<context>->pushasset(g_pSquirrel<context>->m_pSQVM->sqvm, arg.path.c_str(), arg.path.length()); };
}
// Maps
template <ScriptContext context, typename T>
requires is_iterable<T>
inline VoidFunction SQMessageBufferPushArg(T& arg) {
	FunctionVector localv = {};
	localv.push_back([]{g_pSquirrel<context>->newarray(g_pSquirrel<context>->m_pSQVM->sqvm, 0);});
	
	for (const auto& item : arg) {
		SQMessageBufferPushArg<context>(localv, item);
		localv.push_back([]{g_pSquirrel<context>->arrayappend(g_pSquirrel<context>->m_pSQVM->sqvm, -2);});
	}

	return [localv] { for (auto& func : localv) { func(); } };
}
// Vectors
template <ScriptContext context, typename T>
requires is_map<T>
inline VoidFunction SQMessageBufferPushArg(T& map) {
	FunctionVector localv = {};
	localv.push_back([]{g_pSquirrel<context>->newtable(g_pSquirrel<context>->m_pSQVM->sqvm);});
	
	for (const auto& item : map) {
		SQMessageBufferPushArg<context>(localv, item.first);
		SQMessageBufferPushArg<context>(localv, item.second);
		localv.push_back([]{g_pSquirrel<context>->newslot(g_pSquirrel<context>->m_pSQVM->sqvm, -3, false);});
	}

	return [localv]{ for (auto& func : localv) { func(); } };
}

template <ScriptContext context, typename T>
inline void SqRecurseArgs(FunctionVector& v, T& arg) {
	v.push_back(SQMessageBufferPushArg<context>(arg));
}

// This function is separated from the PushArg function so as to not generate too many template instances
// This is the main function responsible for unrolling the argument pack
template <ScriptContext context, typename T, typename... Args>
inline void SqRecurseArgs(FunctionVector& v, T& arg, Args... args) {
	v.push_back(SQMessageBufferPushArg<context>(arg));
	SqRecurseArgs<context>(v, args...);
}

// clang-format on
#endif 

#pragma endregion
