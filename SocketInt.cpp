/*
 * SocketInt.cpp
 *
 *  Created on: Jul 25, 2019
 *      Author: buildrooteclipse
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "config.h"
#include "utilityFunctions.h"
#include "ComputeModulePinOuts2.h"

#include "SocketInt.h"

using namespace std;



#define dbg 1
SocketInt::SocketInt() {


#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif
	try
	{
		this->portOpenStatus = false;
		this->flxEditorRunning = false;
		this->pedalServSock = -1;
		this->editorClntSock = -1;

		this->pedalServAddr.sin_family = AF_INET;
		this->pedalServAddr.sin_addr.s_addr =  inet_addr(pedalServIP);//htonl(INADDR_ANY);
		this->pedalServAddr.sin_port = htons(pedalServPort);
		this->editorClntAddr.sin_family = AF_INET;
		this->editorClntAddr.sin_addr.s_addr =  inet_addr(editorClntIP);//htonl(INADDR_ANY);
		this->editorClntAddr.sin_port = htons(editorClntPort);


		char pinDirection[4] = "in;";
		this->usbDetect = GPIOClass(USB_CONNECTED);
		this->usbDetect.export_gpio();
		this->usbDetect.setdir_gpio(pinDirection);

		this->exitThread = false;

	}
	catch(exception &e)
	{
		cout << "Exception in SocketInt::" << __func__ << ": " << e.what() << endl;
		close(this->pedalServSock);
	}
#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << endl;
#endif

}

SocketInt::~SocketInt()
{

}




#define dbg 0
bool SocketInt::isUsbCableConnected()
{
	bool status = false;
	int usbLine = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif

	usbDetect.getval_gpio(usbLine);
	if(usbLine == 1) status = true;

	this->connectionStatus = status;

#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << ": " <<  status << endl;
#endif
	return status;
}

bool SocketInt::isFlxEditorRunning()
{
	return this->flxEditorRunning;
}


#define dbg 0
int SocketInt::openSocket()
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif

	try
	{
		this->pedalServSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(this->pedalServSock != -1)
		{
#if(dbg >= 2)
			cout << "this->pedalServSock: " << this->pedalServSock << endl;
#endif
			if(bind(this->pedalServSock,(struct sockaddr *) &this->pedalServAddr,
					sizeof(this->pedalServAddr)) != -1)
			{
				errno = 0;
				if(listen(this->pedalServSock, this->queueLimit) != -1)
				{
					cout << "listening for client connection." << endl;
				}
				else
				{
					cout << "SocketInt::openSocket listen error: " <<  strerror(errno) << endl;
				}
			}
			else
			{
				cout << "SocketInt::openSocket failed to bind socket to address: " << strerror(errno) << endl;
			}
		}
		else
		{
			cout << "SocketInt::openSocket failed to create socket: " << strerror(errno) << endl;

			close(this->pedalServSock);

		}

	}
	catch(exception &e)
	{
		cout << "Exception in SocketInt::" << __func__ << ": " << e.what() << endl;
		close(this->pedalServSock);
		status = -1;
	}
#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << endl;
#endif
	return status;
}

int SocketInt::acceptClient(void)
{
	errno = 0;
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif
	this->editorClntLen = sizeof(this->editorClntAddr);
	if((this->editorClntSock = accept(this->pedalServSock,(struct sockaddr *) &this->editorClntAddr, &this->editorClntLen)) != -1)
	{
#if(dbg >= 2)
		cout << "this->editorClntSock: " << this->editorClntSock << endl;
#endif

		this->portOpenStatus = true;
	}
	else
	{
		cout << "SocketInt::acceptClient accept error: " <<  strerror(errno) << endl;
		errno = 0;
		status = -1;
	}
#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << endl;
#endif
	return status;
}

void SocketInt::disconnectFromClient(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif
	errno = 0;
	if(shutdown(this->editorClntSock,SHUT_RDWR) != -1)
	{
		if(close(this->editorClntSock) != -1)
		{
#if(dbg >= 1)
			cout << "client connection closed.  " << endl;
#endif
			this->editorClntSock = -1;
		}
		else
		{
			cout << "failed to close client connection: " << strerror(errno) << endl;
			errno = 0;
		}
	}
	else
	{
		cout << "failed to shutdown client connection: " << strerror(errno) << endl;
		errno = 0;
	}
#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << endl;
#endif

}

#define dbg 0
bool SocketInt::isSocketOpen()
{
#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif
	errno = 0;

#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << endl;
#endif
	return this->portOpenStatus;
}

#define dbg 1
int SocketInt::closeSocket()
{
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: SocketInt::" << __func__ << endl;
#endif
	try
	{
		close(this->editorClntSock);
		close(this->pedalServSock);
		this->portOpenStatus = false;
	}
	catch(exception &e)
	{
		cout << "Exception in SocketInt::" << __func__ << ": " << e.what() << endl;
		status = -1;
	}
#if(dbg >= 1)
	cout << "***** EXITING: SocketInt::" << __func__ << ": " << status << endl;
#endif
	return status;
}






string SocketInt::readNewData(void)
{
	string newData;
	if(this->toPedal.empty() == false)
	{
		newData = this->toPedal.front();
		this->toPedal.pop();
	}
	return newData;
}



void SocketInt::exitTcpLoop(void)
{
	this->exitThread = true;
}


void  SocketInt::writeData(string input)
{
	this->fromPedal.push(input);
}

#define dbg 0
void SocketInt::tcpLoop(void) // thread this in child class
{
	this->openSocket();
	string toPedalStr;
	string fromPedalStr;
	string tempStr;
	int recvMsgSize;
	char readBuffer[RECVBUFSIZE];
	string request;
	while(this->exitThread == false)
	{
		if(this->flxEditorRunning == false)
		{
			errno = 0;
			if(this->acceptClient() != -1)
			{
				this->flxEditorRunning = true;
			}
			else
			{
				cout << "acceptClient error: " << strerror(errno) << endl;
				this->exitThread = true;

			}
		}
		else
		{
			errno = 0;
			memset(readBuffer,'\0',RECVBUFSIZE);

			if((recvMsgSize = recv(this->editorClntSock, readBuffer, RECVBUFSIZE, 0)) != -1)
			{

				for(int i = 0; i < RECVBUFSIZE; i++)
				{
					if(' ' <= readBuffer[i] && readBuffer[i] <= '~')
					{
						request += readBuffer[i];
					}
					else
					{
						break;
					}
				}
				cout << "recvMsgSize: " << recvMsgSize << endl;
				if(recvMsgSize > 995) continue;
				cout << "SocketInt::tcpLoop request: " << request << endl;
				cout << endl;
				memset(readBuffer,'\0',RECVBUFSIZE);
				if(request.compare("exit") == 0)
				{
					request.clear();
					this->flxEditorRunning = false;
					this->disconnectFromClient();
					continue;
				}
				else
				{
					this->toPedal.push(request);
				}

				request.clear();



				while(this->fromPedal.empty() == true);

				fromPedalStr = this->fromPedal.front();
				cout << "tcpLoop->fromPedalStr: " << fromPedalStr << endl;
				this->fromPedal.pop();


				{
					int numBytesSent = send(this->editorClntSock, fromPedalStr.c_str(), fromPedalStr.length(), 0);

					printf("%d:%d\n",fromPedalStr.length(), numBytesSent);
					if (errno == 0)
					{
						fromPedalStr.clear();
						memset(readBuffer,'\0',RECVBUFSIZE);
					}
					else
					{
						printf("error in tcpLoop write: %s\n", strerror(errno));
					}
				}
			}
			else
			{
				cout << "server recv error: " << strerror(errno) << endl;
				this->exitThread = true;
			}
		}
	}
	cout << "exited tcpLoop" << endl;
	this->closeSocket();
}
