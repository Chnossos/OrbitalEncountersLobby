#pragma once

#include <boost/asio/io_service.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace detail
{
	struct IMessageDispatcher
	{
		virtual ~IMessageDispatcher() = 0;
	};

	template<typename Message>
	struct MessageDispatcher : public IMessageDispatcher
	{
		std::vector<std::function<void(std::shared_ptr<Message>)>> handlers;
	};
}

class MessageQueue
{
private:
	boost::asio::io_service & service;

	using DispatcherPtr = std::unique_ptr<detail::IMessageDispatcher>;
	std::unordered_map<std::type_index, DispatcherPtr> _dispatchers;

public:
	MessageQueue(boost::asio::io_service & service);

public:
	template<typename Message, typename F, typename... Args>
	void registerHandler(F && f, Args && ... args);

	template<typename Message, typename... Args>
	void push(Args && ... args);
};

/* *****************************************************************************
** TEMPLATE IMPL
** ****************************************************************************/

template<typename Message, typename F, typename... Args>
inline void MessageQueue::registerHandler(F && f, Args && ... args)
{
	std::type_index const type { typeid(Message) };

	auto it = _dispatchers.find(type);
	if (it == _dispatchers.end())
	{
		it = _dispatchers.emplace
		(
			std::piecewise_construct,
			std::forward_as_tuple(std::move(type)),
			std::forward_as_tuple(std::make_unique<detail::MessageDispatcher<Message>>())
		)
		.first;
	}

	static_cast<detail::MessageDispatcher<Message> *>(it->second.get())->handlers.emplace_back(
		std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1)
	);
}

template<typename Message, typename... Args>
inline void MessageQueue::push(Args && ... args)
{
	std::type_index const type { typeid(Message) };

	auto it = _dispatchers.find(type);
	if (it == _dispatchers.end())
	{
		std::cerr << "WARN No handler for message type '" << type.name() << "'." << std::endl;
		return;
	}

	auto dispatcher = static_cast<detail::MessageDispatcher<Message> *>(it->second.get());
	auto messagePtr = std::make_shared<Message>(std::forward<Args>(args)...);

	for (auto const & handler : dispatcher->handlers)
		service.post(std::bind(handler, messagePtr));
}
