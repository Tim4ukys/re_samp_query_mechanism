#include "SQM.h"

SQM::SQM(const std::string& host, const std::string& port) :
	io_service()
{
	resolver = std::make_unique<t_udp::resolver>(io_service);
	socket = std::make_unique<t_udp::socket>(io_service);

	t_udp::endpoint query_endpoint = *resolver->resolve(host, port);
	translateIP(query_endpoint.address().to_string(), m_arrIP);

	boost::system::error_code err;
	socket->connect(query_endpoint, err);
	if (err) {
		std::cerr << "Don`t connect to server epta. err: " << err.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SQM::sendRequest(const eRequestType tp, void* data, size_t lenData) {
	std::string sendData;
	sendData.resize(11 + lenData);

	LPBYTE pSndData = (PBYTE)sendData.data();
	memcpy(pSndData, "SAMP", 4);
	memcpy(pSndData + 4, m_arrIP, 4);
	memcpy(pSndData + 8, &m_uPort, 2);
	memcpy(pSndData + 0xa, &tp, 1U);
	if (lenData > 0) {
		memcpy(pSndData + 0xb, data, lenData);
	}

	socket->send(boost::asio::buffer(sendData));
}

inline void SQM::translateIP(const std::string& ip, BYTE outByteIP[4])
{
	std::stringstream sstream{ ip };

	for (size_t i{}; i < 4; ++i) {
		std::string temp;
		std::getline(sstream, temp, '.');
		outByteIP[i] = (BYTE)strtol(temp.c_str(), nullptr, 10);
	}
}