#include <cstdio>
#include <cstring>

#include "httplib.h"
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "chatglm.h"
#include <string>
#include "main.h"


std::string GBKToUTF8(const std::string& strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;

}

std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	TCHAR* wszGBK = new TCHAR[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}

int main(int argc, char** argv)
{
	fastllm::SetThreads(8);
	fastllm::ChatGLMModel chatGlm;

	std::string type;
	std::cout << "Use chatglm-6b-int4 or chatglm-6b-int8 ? 8/4 (Default = 4) ";
	std::getline(std::cin, type);
	if (type == "8")
	{
		chatGlm.LoadFromFile("chatglm-6b-int8.bin");
	}
	else if (type == "4" || type == "")
	{
		chatGlm.LoadFromFile("chatglm-6b-int4.bin");
	}

	std::string history = "";
	int round = 0;
	std::stringstream ss;

	std::cout << "Use Web Interface ? Y/N (Default = N) ";
	std::getline(std::cin, type);

	if (type == "Y" || type == "y")
	{
		system("chcp 65001");

		httplib::Server svr;
		std::atomic_bool waiting;
		waiting = false;
		std::string last_request = "";

		auto chat = [&](std::string input)
		{
			if (input == "reset" || input == "stop")
			{
				history = "";
				round = 0;
				ss << "<eop>\n";
			}
			else
			{
				history += ("[Round " + std::to_string(round++) + "]\n问：" + input);
				auto prompt = round > 1 ? history : input;

				waiting = true;
				std::string ret = chatGlm.Response(prompt, &ss, false);
				waiting = false;

				std::cout << "A: " << ret << std::endl;

				history += ("答：" + ret + "\n");
			}
		};

		svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res)
			{
				if (req.body == last_request)
				{
					res.set_content(ss.str(), "text/plain");
					return;
				}
				if (waiting)
				{
					res.set_content(ss.str(), "text/plain");
				}
				else
				{
					ss.str("");
					std::cout << "\nQ: " << req.body << std::endl;
					last_request = req.body;
					std::thread chat_thread(chat, last_request);
					chat_thread.detach();
				}
			});

		svr.set_mount_point("/", "web");
		std::cout << ">>> please open http://localhost:8081\n";
		svr.listen("localhost", 8081);
		std::cout << ">>> end\n";
	}
	else if (type == "N" || type == "n" || type == "")
	{
		while (true)
		{
			std::cout << "\nQ: ";
			std::string input;
			std::getline(std::cin, input);

			if (input == "stop")
			{
				break;
			}
			else if (input == "reset")
			{
				history = "";
				round = 0;
				continue;
			}

			history += ("[Round " + std::to_string(round++) + "]\n问：" + input);
			auto prompt = round > 1 ? history : input;

			std::cout << "A: ";;
			std::string ret = chatGlm.Response(GBKToUTF8(prompt), &ss, true);

			history += ("答：" + ss.str() + "\n");
		}
	}

	return 0;
}
