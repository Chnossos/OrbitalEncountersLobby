#pragma once

#include <OrbitalEncounters/Core/Log.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace detail
{
	/// Base class to enable polymorphism.
	struct IMessageDispatcher
	{
		/// Pure virtual destructor to disable direct instantiation.
		virtual ~IMessageDispatcher() = 0;
	};

	/// Real instantiation of a message dispatcher for a single message @c T.
	template<typename T>
	struct MessageDispatcher : public IMessageDispatcher
	{
		using Message = std::shared_ptr<T>;
		using Handler = std::function<void(Message)>;

		/// Collection of handlers for message @c T.
		std::vector<Handler> handlers;
	};
}

/// Decoupling pattern: Register handlers and send them messages.
class MessageQueue
{
private:
	boost::asio::io_context & _service;

	using DispatcherPtr = std::unique_ptr<detail::IMessageDispatcher>;
	/// Mapping between a message type and its handler collection.
	std::unordered_map<std::type_index, DispatcherPtr> _dispatchers;

public:
	/// Constructor.
	MessageQueue(boost::asio::io_context & service);

	/// Default virtual destructor.
	virtual ~MessageQueue() = default;

public:
	/// Register a new handler for messages of type @c T.
	template<typename T, typename F, typename... Args>
	void registerHandler(F && f, Args && ... args);

	/// Push a new message of type @c T initialized with @c args.
	template<typename T, typename... Args>
	void push(Args && ... args);
};

/**
 * @param[in]  f     Callable object passed to @c std::bind.
 * @param[in]  args  Arguments forwarded to @c std::bind.
 *
 * @tparam     T          Message type.
 * @tparam     F          Callable object type.
 * @tparam     Args       Arguments type.
 *
 * @remark     Handlers signature take at least a single @c std::shared_ptr<T>
 *             as its **last** argument. You don't have to use an explicit
 *             placeholder.
 *
 * @par Example
 *
 * Considering this class:
 *
 * @code{.cpp}
 * class SomeClass
 * {
 * public:
 *     void messageHandler(std::shared_ptr<MessageType> msg);
 * };
 * @endcode
 *
 * We register the member function as a handler like this:
 * @code{.cpp}
 * registerHandler<MessageType>(&SomeClass::messageHandler, this);
 * @endcode
 */
template<typename T, typename F, typename... Args>
inline void MessageQueue::registerHandler(F && f, Args && ... args)
{
	using Dispatcher = detail::MessageDispatcher<T>;

	std::type_index const type { typeid(T) };

	auto it = _dispatchers.find(type);
	if (it == _dispatchers.end())
	{
		it = _dispatchers.emplace
		(
			std::piecewise_construct,
			std::forward_as_tuple(std::move(type)),
			std::forward_as_tuple(std::make_unique<Dispatcher>())
		)
		.first;
	}

	namespace pch   = std::placeholders;
	auto dispatcher = static_cast<Dispatcher *>(it->second.get());
	dispatcher->handlers.emplace_back(
		std::bind(std::forward<F>(f), std::forward<Args>(args)..., pch::_1)
	);
}

/**
 * @param[in]  args  Arguments forwarded to the instance of the message.
 *
 * @tparam     T          Message type.
 * @tparam     Args       Arguments type.
 */
template<typename T, typename... Args>
inline void MessageQueue::push(Args && ... args)
{
	std::type_index const type { typeid(T) };

	auto it = _dispatchers.find(type);
	if (it != _dispatchers.end())
	{
		using Dispatcher = detail::MessageDispatcher<T>;
		auto dispatcher  = static_cast<Dispatcher *>(it->second.get());
		auto messagePtr  = std::make_shared<T>(std::forward<Args>(args)...);

		for (auto const & handler : dispatcher->handlers)
			boost::asio::post(_service, std::bind(handler, messagePtr));
	}
	else
		Log { std::cerr } << "WARN No handler for message type '"
		                  << type.name() << "'.\n";
}
