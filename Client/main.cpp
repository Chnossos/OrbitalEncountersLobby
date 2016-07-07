#include <boost/asio.hpp>
#include <clocale>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

namespace pch = std::placeholders;

bool onError(boost::system::error_code const & ec)
{
	namespace errc = boost::system::errc;

	switch (ec.default_error_condition().value())
	{
		case errc::success:
			return false;

		case errc::no_such_file_or_directory:
		//case errc::connection_reset:
		{
			std::cerr << "\rERROR: Oops, server disconnected...\n";
			break;
		}

		default:
		{
			std::cerr << "\rERROR " << ec.value()
				<< '(' << ec.default_error_condition().value()
				<< ") : " << ec.message() << '\n';
			break;
		}
	}
	std::cout << "Press Enter to exit...";
	return true;
}

void onPacketReceived(boost::asio::ip::tcp::socket &    socket,
					  boost::asio::streambuf &          buffer,
					  boost::system::error_code const & ec)
{
	if (onError(ec))
	{
		socket.get_io_service().stop();
		return;
	}

	std::string packet;
	std::istream is { &buffer };
	std::getline(is, packet, '\0');
	std::cout << "\rrecv: <" << packet << ">\n> " << std::flush;

	boost::asio::async_read_until(socket, buffer, '\0',
		std::bind(&onPacketReceived, std::ref(socket),
					std::ref(buffer), pch::_1)
	);
}

void onPacketSent(std::shared_ptr<std::string> pkt,
				  boost::system::error_code const & ec, size_t)
{
	if (onError(ec))
		return;

	pkt->pop_back();
	std::cout << "\rsent: <" << *pkt << ">\n> ";
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

	std::cout << "> ";

	std::string cmd;
	while (!ios.stopped() && std::getline(std::cin, cmd) && cmd != "exit")
	{
		if (!ios.stopped() && cmd.size())
		{
			auto msg = std::make_shared<std::string>(cmd + '\0');
			boost::asio::async_write(
				socket, boost::asio::buffer(*msg),
				std::bind(onPacketSent, msg, pch::_1, pch::_2));

			std::cout << "> ";
		}
	}

	bool fromError = false;

	if (ios.stopped())
		fromError = true;
	else
		ios.stop();

	worker.join(); // Must join() before exiting

	if (!fromError)
	{
		std::cout << "Press Enter...";
		std::cin.get();
	}
}