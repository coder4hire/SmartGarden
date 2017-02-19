#include "DomoticzInterface.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

CDomoticzInterface::CDomoticzInterface(const char* domoticzIP, unsigned short port)
{
	this->domoticzIP = domoticzIP;
	this->port = port;

}

CDomoticzInterface::~CDomoticzInterface()
{
}

bool CDomoticzInterface::SendSwitchCommand(int idx, bool isEnabled)
{
	/* Now specify the POST data */
	std::string str = domoticzIP+ "/json.htm?type=command&param=switchlight&idx=";
	char strIdx[32];
	sprintf(strIdx, "%d", idx);

	str += strIdx;
	str += "&switchcmd=";
	str += (isEnabled ? "On" : "Off");
	str += "&level=0&passcode=";
	
	// TODO: Process response
	std::string resp = SendHTTPRequest(str);
	printf("%s", resp.c_str());

	return false;
}

std::string CDomoticzInterface::SendHTTPRequest(std::string& request)
{
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) 
	{
		return "";
	}

	std::string strIP = GetIP(domoticzIP);
	sockaddr_in remote;
	remote.sin_family = AF_INET;
	int tmpRes = inet_pton(AF_INET, strIP.c_str(), (void *)(&(remote.sin_addr.s_addr)));
	if (tmpRes <= 0)
	{
		return "";
	}
	remote.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&remote, sizeof(struct sockaddr)) < 0) 
	{
		return "";
	}
	
	std::string getQuery = BuildGetQuery(domoticzIP, request);

	//Send the query to the server
	int sent = 0;
	while (sent < getQuery.size())
	{
		tmpRes = send(sock, getQuery.c_str() + sent, getQuery.size() - sent, 0);
		if (tmpRes <0) 
		{
			return "";
		}
		sent += tmpRes;
	}
	
	//now it is time to receive the page
	char buf[1024] = {};

	std::string htmlContent;
	
	while ((tmpRes = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
	{
		buf[tmpRes] = 0;
		htmlContent += buf;
		if (tmpRes < sizeof(buf) - 1);
		{
			break;
		}
	}

	int htmlStart = htmlContent.find("\r\n\r\n");

	if (htmlStart != -1)
	{
		htmlContent = htmlContent.substr(htmlStart);
	}
	close(sock);
	return htmlContent;
}

std::string CDomoticzInterface::GetIP(std::string& host)
{
	struct hostent *hent;
	char ip[16] = {};
	if ((hent = gethostbyname(host.c_str())) == NULL)
	{
		return "";
	}
	if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, sizeof(ip)-1) == NULL)
	{
		return "";
	}
	return ip;
}

std::string CDomoticzInterface::BuildGetQuery(const std::string& host, const std::string& page)
{
	std::string query = "GET "+page+" HTTP/1.0\r\nHost: "+host+"\r\nUser-Agent: LCDPANEL\r\n\r\n";
	return query;
}
