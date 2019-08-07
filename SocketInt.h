/*
 * SocketInt.h
 *
 *  Created on: Jul 25, 2019
 *      Author: buildrooteclipse
 */

#ifndef SOCKETINT_H_
#define SOCKETINT_H_

using std::string;

#define BACKLOG 10 // how many pending connections queue will hold
#define RECVBUFSIZE 1000
#define XMITBUFSIZE 40000

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "GPIOClass.h"
#include <thread>
#include <queue>




class SocketInt {
private:
	bool connectionStatus;
	bool flxEditorRunning;
	bool portOpenStatus;
	bool _openPort;
	int pedalServSock;
	int editorClntSock;
	unsigned int  editorClntLen;
	struct sockaddr_in editorClntAddr;
	struct sockaddr_in pedalServAddr;
	const char *pedalServIP = "192.168.10.33";
	unsigned int pedalServPort = 8000;
	const char *editorClntIP = "192.168.10.30";
	unsigned int editorClntPort = 8000;
	int queueLimit = BACKLOG;
	std::queue<string> toPedal;
	std::queue<string> fromPedal;

	bool exitThread;
	GPIOClass usbDetect;
	//int commState; // 0: not open (listening) , 1: open, connected

	int openSocket();
	int acceptClient(void);
	void disconnectFromClient(void);
	int closeSocket();
public:
	SocketInt();
	virtual ~SocketInt();

	bool isUsbCableConnected();
	bool isFlxEditorRunning();
	bool isSocketOpen();
	string readNewData(void);
	void exitTcpLoop(void);
	void writeData(string input);
	void tcpLoop(void);

};



#endif /* SOCKETINT_H_ */
