#include "pch.h"
#include <iostream>
#include <windows.h>
#include <iostream>
#include <WtsApi32.h>
#pragma comment(lib, "WtsApi32.lib")
#include <atlstr.h>
#include <string>
using std::wstring;
#include <vector>
using std::vector;

enum WindowsSessionState {
	Active,              // User logged on to WinStation
	Connected,           // WinStation connected to client
	ConnectQuery,        // In the process of connecting to client
	Shadow,              // Shadowing another WinStation
	Disconnected,        // WinStation logged on without client
	Idle,                // Waiting for client to connect
	Listen,              // WinStation is listening for connection
	Reset,               // WinStation is being reset
	Down,                // WinStation is down due to error
	Init,                // WinStation in initialization
};

struct WindowsSession
{
	int ID;
	wstring Name;
	wstring Domain;
	wstring UserName;
	WindowsSessionState State;
};

vector<WindowsSession> GetWindowsSessions()
{
	vector<WindowsSession> sessions;
	PWTS_SESSION_INFO pSessions = NULL;
	DWORD dwCount = 0;

	WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessions, &dwCount);		
	for (DWORD i = 0; i < dwCount; ++i)
	{
		WindowsSession _session;
		_session.ID = pSessions[i].SessionId;
		_session.Name = pSessions[i].pWinStationName;
		LPWSTR pBuffer = NULL;
		DWORD dwBufSize = 0;

		switch (pSessions[i].State)
		{
		case WTSActive:
			_session.State = Active;
			break;
		case WTSConnected:
			_session.State = Connected;
			break;
		case WTSConnectQuery:
			_session.State = ConnectQuery;
			break;
		case WTSShadow:
			_session.State = Shadow;
			break;
		case WTSDisconnected:
			_session.State = Disconnected;
			break;
		case WTSIdle:
			_session.State = Idle;
			break;
		case WTSListen:
			_session.State = Listen;
			break;
		case WTSReset:
			_session.State = Reset;
			break;
		case WTSDown:
			_session.State = Down;
			break;
		case WTSInit:
			_session.State = Init;
			break;
		}

		WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, pSessions[i].SessionId, WTSDomainName, &pBuffer, &dwBufSize);
		_session.Domain = pBuffer;
		WTSFreeMemory(pBuffer);

		WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, pSessions[i].SessionId, WTSUserName, &pBuffer, &dwBufSize);
		_session.UserName = pBuffer;
		WTSFreeMemory(pBuffer);

		sessions.push_back(_session);
	}
	WTSFreeMemory(pSessions);
	return sessions;
}

int main(int argc, CHAR* argv[])
{
	auto sessions = GetWindowsSessions();
	if (sessions.empty())
	{
		std::cout << "No sessions available" << std::endl;
		return 0;
	}
	std::wcout << "Session\t\tName\t\tState\t\tUserName" << std::endl;
	for (auto session : sessions)
	{
		std::wcout << session.ID << "\t\t" << session.Name << "  \t";
		std::wcout << session.State << "\t\t" << session.Domain << "\\" << session.UserName;
		std::wcout << std::endl;
	}
	return 0;
}