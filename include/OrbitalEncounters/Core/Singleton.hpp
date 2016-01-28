#pragma once

#include <memory>

template<typename T>
class Singleton
{
private:
	static std::unique_ptr<T> _instance;

protected:
	Singleton() = default;
	Singleton(Singleton const &) = delete;
	Singleton(Singleton &&) = delete;
	Singleton & operator=(Singleton const &) = delete;
	Singleton & operator=(Singleton &&) = delete;
	virtual ~Singleton() = default;

public:
	template<typename... Args>
	static void create(Args && ... args);

	static auto instance() -> T &;

	static void release();
};

/* ****************************************************************************
** TEMPLATE IMPL
** ***************************************************************************/

template<typename T>
template<typename... Args>
inline void Singleton<T>::create(Args && ... args)
{
	_instance.reset(new T { std::forward<Args>(args)... });
}

template<typename T>
inline auto Singleton<T>::instance() -> T &
{
	return *_instance;
}

template<typename T>
inline void Singleton<T>::release()
{
	_instance.reset();
}

template<typename T>
std::unique_ptr<T> Singleton<T>::_instance = nullptr;