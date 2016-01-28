#include <OrbitalEncounters/Application.hpp>
#include <iostream>

int main()
{
	Application::create();
	Application::instance().run();
	Application::release();

	std::cout << "Press Enter key to exit...";
	std::cin.get();
}