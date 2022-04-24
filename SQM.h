#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <vector>

#include <boost/asio.hpp>

class SQM {
public:

	SQM(const std::string& host, const std::string& port);

	enum class eRequestType : char {
		SERVERINFO = 'i',
		SERVERRULES = 'r',
		SERVERPING = 'p',
		PLAYERLIST = 'c'
	};

	struct stServerInfo {
		bool m_bIsLocked;
		uint16_t m_nPlayers;
		uint16_t m_nMaxPlayers;
		std::string m_sHostName;
		std::string m_sGameMode;
		std::string m_sLanguage;
	};

	struct stServerRules {
		std::map<std::string, std::string> m_arrRules;
	};

	struct stPlayerList {
		struct stPlayerInfo {
			std::string m_sName;
			int m_nScore;
		};
		//std::map<std::string, stPlayerInfo> m_arrPlayer;
		std::vector<stPlayerInfo> m_arrPlayer;
	};

	struct stServerPing {
		union {
			std::uint32_t m_nNumb;
			std::uint8_t m_arrNumbs[4];
		} m_unRandNumb;
	};

	void sendRequest(const eRequestType tp, void* data = nullptr, size_t lenData = 0u);

private:

	boost::asio::io_service io_service;

	using t_udp = boost::asio::ip::udp;
	std::unique_ptr<t_udp::resolver> resolver;
	std::unique_ptr<t_udp::socket> socket;

	inline void translateIP(const std::string& ip, BYTE outByteIP[4]);

	template<typename T>
	void read(LPBYTE& read_buff, T& out)
	{
		memcpy(PVOID(&out), read_buff, sizeof(T));
		read_buff += sizeof(T);
	}
	template<typename lenStrType>
	void readStr(LPBYTE& read_buff, std::string& out)
	{
		lenStrType len;
		read<lenStrType>(read_buff, len);

		out.resize(std::size_t(len));
		memcpy(PVOID(out.data()), read_buff, std::size_t(len));
		read_buff += size_t(len);
	}

public:

	template<typename T>
	T receive()
	{
		const size_t BUFF_LEN = 2048;
		char buff[BUFF_LEN];
		socket->receive(boost::asio::buffer(buff, BUFF_LEN));
		auto cursorRead = (LPBYTE)buff;

		cursorRead += 11;
		if constexpr (std::is_same_v<T, stServerInfo>) {
			T res;
			read<bool>(cursorRead, res.m_bIsLocked);
			read<unsigned short>(cursorRead, res.m_nPlayers);
			read<unsigned short>(cursorRead, res.m_nMaxPlayers);
			readStr<unsigned int>(cursorRead, res.m_sHostName);
			readStr<unsigned int>(cursorRead, res.m_sGameMode);
			readStr<unsigned int>(cursorRead, res.m_sLanguage);
			return res;
		} 
		else if constexpr (std::is_same_v<T, stServerRules>) {
			T res;
			std::uint16_t len;
			read<unsigned short>(cursorRead, len);
			for (size_t i{}; i < len; i++) {
				std::string key;
				readStr<std::uint8_t>(cursorRead, key);
				readStr<std::uint8_t>(cursorRead, res.m_arrRules[key]);
			}
			return res;
		}
		else if constexpr (std::is_same_v<T, stPlayerList>) {
			T res;
			std::uint16_t len;
			read<unsigned short>(cursorRead, len);
			res.m_arrPlayer.resize(size_t(len));
			for (auto& pl : res.m_arrPlayer) {
				readStr<std::uint8_t>(cursorRead, pl.m_sName);
				read<int>(cursorRead, pl.m_nScore);
			}
			return res;
		} 
		else if constexpr (std::is_same_v<T, stServerPing>) {
			T res;
			read<unsigned int>(cursorRead, res.m_unRandNumb.m_nNumb);
			return res;
		}
		else {
			std::cerr << "Tu eban'ko??" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

protected:

	BYTE m_arrIP[4]; // 255.255.255.255
	USHORT m_uPort; // 7777
};