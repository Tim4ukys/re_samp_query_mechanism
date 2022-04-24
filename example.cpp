#include <iostream>
#include <random>
#include <ctime>

#include "SQM.h"

// https://www.blast.hk/threads/781/post-128505
int randomInteger(int min, int max)
{
	static std::random_device rd;
	static std::default_random_engine e1(rd());
	std::uniform_int_distribution<int> uniform_dist(min, max);
	return uniform_dist(e1);
}

int main(int argc, char* argv[])
{
	SQM sm{"01.gtarp.ru", "7777"};

	std::cout << "----- INFO SERVER -----" << std::endl;
	sm.sendRequest(SQM::eRequestType::SERVERINFO);
	auto inf = sm.receive<SQM::stServerInfo>();
	std::cout << "Host name: " << inf.m_sHostName << std::endl
		<< "Player: " << inf.m_nPlayers << "/" << inf.m_nMaxPlayers << std::endl;

	std::cout << "----- RULES -----" << std::endl;
	sm.sendRequest(SQM::eRequestType::SERVERRULES);
	auto rul = sm.receive<SQM::stServerRules>();
	for (const auto& [rkey, rr] : rul.m_arrRules) {
		std::cout << rkey.c_str() << ": " << rr.c_str() << std::endl;
	}

	std::cout << "----- PLAYERS -----" << std::endl;
	sm.sendRequest(SQM::eRequestType::PLAYERLIST);
	auto players = sm.receive<SQM::stPlayerList>();
	size_t i_player{};
	for (const auto& pl : players.m_arrPlayer) {
		std::cout << "Nick: " << pl.m_sName << "; Score: " << pl.m_nScore << std::endl;
		if (++i_player > 5) {
			std::cout << "..." << std::endl;
			break;
		}
	}

	std::cout << "----- PING -----" << std::endl;
	int rand = randomInteger(INT_MIN, INT_MAX);
	time_t tick = time(nullptr);
	sm.sendRequest(SQM::eRequestType::SERVERPING, &rand, 4);
	sm.receive<SQM::stServerPing>();
	tick = time(nullptr) - tick;
	std::cout << "Ping: " << (int)tick << std::endl;



	return EXIT_SUCCESS;
}
