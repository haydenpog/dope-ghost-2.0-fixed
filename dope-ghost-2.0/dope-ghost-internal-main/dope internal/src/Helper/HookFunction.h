#pragma once

class HookFunctionBase
{
private:
	HookFunctionBase* m_next;

public:
	HookFunctionBase()
	{
		Register();
	}

	~HookFunctionBase() = default;

	virtual void Run() = 0;

	static void RunAll();
	void Register();
};

class HookFunction : public HookFunctionBase
{
private:
	void(*m_function)();

public:
	HookFunction(void(*function)())
	{
		m_function = function;
	}

	~HookFunction() = default;

	virtual void Run() override
	{
		m_function();
	}
};