#include <OrbitalEncounters/Core/MessageQueue.hpp>

// A pure virtual destructor still needs an implementation.
detail::IMessageDispatcher::~IMessageDispatcher() = default;

/**
 * @param      service  The @c io_context used for posting handlers.
 */
MessageQueue::MessageQueue(boost::asio::io_context & service)
: _service { service }
{}
