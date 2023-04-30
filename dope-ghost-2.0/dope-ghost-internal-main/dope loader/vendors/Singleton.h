#pragma once

template<typename T>
class Instance {
protected:
	Instance() { }
	~Instance() { }

	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;

	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;
public:
	static auto Get() {
		static T inst{};
		return &inst;
	}
};