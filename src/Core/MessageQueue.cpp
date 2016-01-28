#include <OrbitalEncounters/Core/MessageQueue.hpp>

detail::IMessageDispatcher::~IMessageDispatcher() = default;

MessageQueue::MessageQueue(boost::asio::io_service & service)
: service { service }
{}