#include <iostream>

/*
@Pierre je précise un peu le "CDC" de ta partie : 

- coder le MasterServerLobby (le serveur principal qui va lister les partie publique)
(la techno est a ta préférence)

- coder le Script c# pour le GameObject Unity qui va se connecter à ton serveur, via
des fonctions publiques ce prefab sera capable de : 

* se co directement a ton serveur (une variable public pour set l'adresse IP fixe du
serveur physique qu'on a pas encore)

* dire à ton serveur que l'on crée une nouvelle partie (en transférant le nom de la
partie, l'adresse Ip du joueur qui héberge cette partie, un enum pour indiquer le 
type de partie, et la limite de joueur max)

* mettre à jour sur ton serveur, le nombre de joueurs actuellement  présent dans la
partie

* supprimer la même partie que le gameobject a demandé au serveur d'ajouter à sa 
liste (ou alors libre à toi de faire un systeme de timeout et de ping régulier pour 
dire au serveur que la partie est toujours là et du coup au passage on peut updater
qq infos de game)

* récupérer l'ensemble de toute les parties actuellement dans la liste du serveur
avec toutes leurs info (ip, mode de jeu, nom de la game, etc etc)

tu l'a compris, à la fin, tu n'aura plus qu'à me package ce gameobject et son script
et on aura plus qu'à le drag and drop dans notre scène pour pouvoir interagir depuis
notre projet avec ton serveur
*/

#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <OrbitalEncounters/Network/SessionManager.hpp>
#include <boost/asio/signal_set.hpp>
#include <clocale>
#include <csignal>
#include <functional>
#include <iostream>

// Nobody but main is supposed to access this class
class Application final
{
public:
	Application();
	~Application();

public:
	void run();

private:
	void onSignal(boost::system::error_code const & ec);
};

int main()
{
	// We want French accents working when the system locale is French
	std::setlocale(LC_ALL, "");

	Application {} .run();

	std::cout << "Press Enter key to exit...";
	std::cin.get();
}

Application::Application()
{
	ServiceLocator::add<ThreadPool>();

	auto & tp = ServiceLocator::get<ThreadPool>();
	// TODO: std::this_thread::hardware_concurrency balancing
	tp.spawn({
		{     "App", 0 },
		{ "Network", 3 }
	});

	// Don't forget to del() in the destructor!

	ServiceLocator::add<SocketListener>(tp["Network"].service);
	ServiceLocator::add<SessionManager>();
	ServiceLocator::add<Lobby>();
}

void Application::run()
{
	namespace pch = std::placeholders;

	if (!ServiceLocator::get<SocketListener>().listen(4242))
	{
		Log { std::cerr } << "Cannot start the server.\n";
		return;
	}

	auto & tp = ServiceLocator::get<ThreadPool>();

	boost::asio::signal_set signalSet { tp["App"].service, SIGINT };
	signalSet.async_wait(std::bind(&Application::onSignal, this, pch::_1));

	Log {} << "System ready on *:4242\n"
	       << "Press CTRL-C to stop the application...\n";
	tp["App"].service.run();

	Log {} << "Releasing the network threads.\n";
	tp["Network"].shutdown();
}

Application::~Application()
{
	ServiceLocator::del<Lobby>();
	ServiceLocator::del<SessionManager>();
	ServiceLocator::del<ThreadPool>();
}

void Application::onSignal(boost::system::error_code const &)
{
	Log {} << "Shutdown requested...\n";

	Log {} << "Preventing incoming clients from connecting\n";
	ServiceLocator::get<SocketListener>().close();

	Log {} << "Releasing the main thread\n";
	ServiceLocator::get<ThreadPool>()["App"].shutdown();
}
