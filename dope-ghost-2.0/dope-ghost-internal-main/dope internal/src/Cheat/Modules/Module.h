#pragma once
#include <vector>
#include <thread>

class Module
{
public:
	virtual ~Module() = default;

	virtual void Run(JNIEnv *env) {}
	virtual void OnRender(JNIEnv* env) {}
	virtual void OnImGuiRender(JNIEnv* env) {}
	virtual void OnReceiveData() {}
	virtual void OnPlayerPreRenderCallback() {}
	virtual void OnPlayerPostRenderCallback() {}
};

class Modules
{
private:
	static std::vector<Module*> m_Modules;
	static std::vector<std::thread> m_ModuleThread;

public:
	static void InitializeModules();
	static void DestroyModules();

	static const auto& GetRegisteredModules() {
		return m_Modules;
	}
};