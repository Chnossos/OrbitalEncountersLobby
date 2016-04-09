#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <clocale>
#include <thread>

namespace pch = std::placeholders;

void onPacketReceived(boost::asio::ip::tcp::socket &    socket,
					  boost::asio::streambuf &          buffer,
					  boost::system::error_code const & ec)
{
	if (ec)
	{
		std::cerr << "FAIL: " << ec.message() << std::endl;
		socket.get_io_service().stop();
	}
	else
	{
		std::string packet;
		std::istream is { &buffer };
		std::getline(is, packet, '\0');
		std::cout << "recv: <" << packet << '>' << std::endl;

		boost::asio::async_read_until(socket, buffer, '\0',
			std::bind(&onPacketReceived, std::ref(socket),
					  std::ref(buffer), pch::_1)
		);
	}
}

void onPacketSent(boost::system::error_code const & ec, size_t)
{
	if (ec)
		std::cerr << __FUNCTION__ << ": " << ec.message() << std::endl;
	else
		std::cout << "sent!" << std::endl;
}

void doWork(boost::asio::io_service & ios)
{
	ios.run(); // Exec all handlers as they come
}

int main()
{
	// Enable French accents
	std::setlocale(LC_ALL, "");

	// Almost every class in asio needs it
	boost::asio::io_service ios;
	// Prevent run() from returning when out of work
	boost::asio::io_service::work work { ios };

	boost::asio::ip::tcp::endpoint endpoint {
		boost::asio::ip::address::from_string("127.0.0.1"), 4242 };
	boost::asio::ip::tcp::socket socket { ios };
	// For reading purpose
	boost::asio::streambuf buffer;

	// Connect to the server
	boost::system::error_code ec;
	if (socket.connect(endpoint, ec))
	{
		std::cerr << ec.message();
		std::cin.get();
		return 1;
	}

	// Read/Write operations in a different thread
	std::thread worker { std::bind(&doWork, std::ref(ios)) };

	// Start async read
	boost::asio::async_read_until(socket, buffer, '\0',
	    std::bind(&onPacketReceived, std::ref(socket), std::ref(buffer), pch::_1)
	);

	std::string cmd, tmp;
	while (!ios.stopped() && std::getline(std::cin, cmd) && cmd != "exit")
	{
		if (!ios.stopped() && cmd.size())
		{
			tmp = cmd + '\0';
			std::cout << "Sending: <" << cmd << ">" << std::endl;
			boost::asio::async_write(socket, boost::asio::buffer(tmp),
									 std::bind(onPacketSent, pch::_1, pch::_2));
		}
	}

	ios.stop();
	worker.join(); // Must join() before exiting

	std::cout << "Press Enter...";
	std::cin.get();
}