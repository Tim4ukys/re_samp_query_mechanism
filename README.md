# re_samp_query_mechanism

Он нужен для того, чтобы получить какую-либо информацию о сервере(общую информацию, кол-во игроков и их ники, правила сервера и т.д.).<br>
В качестве сетевого движка используется boost::asio.

---
## Пример
```C++
  SQM sm{"01.gtarp.ru", "7777"}; // Не обязательно указывать IP, можно и ввиде сайта
  
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
```
## Вывод
```
----- INFO SERVER -----
Host name: GTA RolePlay 01 [x2 Day]
Player: 50/1000
----- RULES -----
lagcomp: On
mapname: Russia
version: 0.3.7-R2
weather: 1
weburl: gtarp.ru
worldtime: 09:00
```
