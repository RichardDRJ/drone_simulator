// This file is part of the REMOTE API
// 
// Copyright 2006-2013 Dr. Marc Andreas Freese. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// The REMOTE API is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.0.4 on July 8th 2013

#include "extApiPlatform.h"
#include <stdio.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
	#pragma message("Adding library: Winmm.lib")
	#pragma comment(lib,"Winmm.lib")
	#pragma message("Adding library: Ws2_32.lib")
	#pragma comment(lib,"Ws2_32.lib")
	#define MUTEX_HANDLE HANDLE
	#define MUTEX_HANDLE_X MUTEX_HANDLE
	#define THREAD_ID DWORD
	WSADATA	_socketWsaData;
#elif defined (__linux) || defined (__APPLE__)
	#include <pthread.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <netinet/in.h>
	#include <sys/time.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#define MUTEX_HANDLE pthread_mutex_t
	#define MUTEX_HANDLE_X MUTEX_HANDLE*
	#define THREAD_ID pthread_t
	#define SOCKET int
	#define DWORD unsigned long
	#define INVALID_SOCKET (-1)
#endif

MUTEX_HANDLE _globalMutex;

MUTEX_HANDLE _mutex1[MAX_EXT_API_CONNECTIONS];
MUTEX_HANDLE _mutex1Aux[MAX_EXT_API_CONNECTIONS];
simxInt _mutex1LockLevel[MAX_EXT_API_CONNECTIONS];
THREAD_ID _lock1ThreadId[MAX_EXT_API_CONNECTIONS];

MUTEX_HANDLE _mutex2[MAX_EXT_API_CONNECTIONS];
MUTEX_HANDLE _mutex2Aux[MAX_EXT_API_CONNECTIONS];
simxInt _mutex2LockLevel[MAX_EXT_API_CONNECTIONS];
THREAD_ID _lock2ThreadId[MAX_EXT_API_CONNECTIONS];

SOCKET _socketConn[MAX_EXT_API_CONNECTIONS];
struct sockaddr_in _socketServer[MAX_EXT_API_CONNECTIONS];

simxShort extApi_endianConversionShort(simxShort shortValue)
{ /* just used for testing purposes. Endianness is detected on the server side */
#ifdef ENDIAN_TEST
	simxShort retV;
	((char*)&retV)[0]=((char*)&shortValue)[1];
	((char*)&retV)[1]=((char*)&shortValue)[0];
	return(retV);
#else
	return(shortValue);
#endif
}

simxUShort extApi_endianConversionUShort(simxUShort shortValue)
{ /* just used for testing purposes. Endianness is detected on the server side */
#ifdef ENDIAN_TEST
	simxUShort retV;
	((char*)&retV)[0]=((char*)&shortValue)[1];
	((char*)&retV)[1]=((char*)&shortValue)[0];
	return(retV);
#else
	return(shortValue);
#endif
}

simxInt extApi_endianConversionInt(simxInt intValue)
{ /* just used for testing purposes. Endianness is detected on the server side */
#ifdef ENDIAN_TEST
	simxInt retV;
	((char*)&retV)[0]=((char*)&intValue)[3];
	((char*)&retV)[1]=((char*)&intValue)[2];
	((char*)&retV)[2]=((char*)&intValue)[1];
	((char*)&retV)[3]=((char*)&intValue)[0];
	return(retV);
#else
	return(intValue);
#endif
}

simxFloat extApi_endianConversionFloat(simxFloat floatValue)
{ /* just used for testing purposes. Endianness is detected on the server side */
#ifdef ENDIAN_TEST	
	simxFloat retV;
	((char*)&retV)[0]=((char*)&floatValue)[3];
	((char*)&retV)[1]=((char*)&floatValue)[2];
	((char*)&retV)[2]=((char*)&floatValue)[1];
	((char*)&retV)[3]=((char*)&floatValue)[0];
	return(retV);
#else
	return(floatValue);
#endif
}

simxDouble extApi_endianConversionDouble(simxDouble doubleValue)
{ /* just used for testing purposes. Endianness is detected on the server side */
#ifdef ENDIAN_TEST	
	simxDouble retV;
	((char*)&retV)[0]=((char*)&doubleValue)[7];
	((char*)&retV)[1]=((char*)&doubleValue)[6];
	((char*)&retV)[2]=((char*)&doubleValue)[5];
	((char*)&retV)[3]=((char*)&doubleValue)[4];
	((char*)&retV)[4]=((char*)&doubleValue)[3];
	((char*)&retV)[5]=((char*)&doubleValue)[2];
	((char*)&retV)[6]=((char*)&doubleValue)[1];
	((char*)&retV)[7]=((char*)&doubleValue)[0];
	return(retV);
#else
	return(doubleValue);
#endif
}

simxInt extApi_getTimeInMs()
{
#ifdef _WIN32
	return(timeGetTime()&0x03ffffff);
#elif defined (__linux) || defined (__APPLE__)
	struct timeval tv;
	DWORD result=0;
	if (gettimeofday(&tv,NULL)==0)
		result=(tv.tv_sec*1000+tv.tv_usec/1000)&0x03ffffff;
	return(result);
#endif
}

simxInt extApi_getTimeDiffInMs(simxInt lastTime)
{
	simxInt currentTime=extApi_getTimeInMs();
	if (currentTime<lastTime)
		return(currentTime+0x03ffffff-lastTime);
	return(currentTime-lastTime);
}

simxVoid extApi_initRand()
{
	srand(extApi_getTimeInMs());
}

simxFloat extApi_rand()
{
	return(((float)rand())/((float)RAND_MAX));
}

simxVoid extApi_sleepMs(simxInt ms)
{
#ifdef _WIN32
	Sleep(ms);
#elif defined (__linux) || defined (__APPLE__)
	usleep(ms*1000);
#endif
}

simxVoid extApi_switchThread()
{ /* or just use a extApi_sleepMs(1) here */
	extApi_sleepMs(1);
/*
#ifdef _WIN32
	extApi_sleepMs(1);
#elif defined (__APPLE__)
	pthread_yield_np();
#elif defined (__linux)
	pthread_yield();
#endif
*/
}

simxChar extApi_areStringsSame(const simxChar* str1,const simxChar* str2)
{
	if (strcmp(str1,str2)==0)
		return(1);
	return(0);
}

simxInt extApi_getStringLength(const simxChar* str)
{
	return((simxInt)strlen(str));
}

simxChar* extApi_readFile(const simxChar* fileName,simxInt* len)
{
	FILE *file;
	unsigned long fileLength;
	simxChar* retVal=0;
	file=fopen(fileName,"rb");
	len[0]=0;
	if (file)
	{
		fseek(file,0,SEEK_END);
		fileLength=ftell(file);
		fseek(file,0,SEEK_SET);
		retVal=extApi_allocateBuffer(fileLength);
		fread(retVal,fileLength,1,file);
		fclose(file);
		len[0]=fileLength;
	}
	return(retVal);
}

simxChar* extApi_allocateBuffer(simxInt bufferSize)
{
	return ((simxChar*) (malloc(bufferSize)));
}

simxVoid extApi_releaseBuffer(simxChar* buffer)
{
	free(buffer);
}

simxVoid extApi_createMutexes(simxInt clientID)
{
#ifdef _WIN32
	_mutex1[clientID]=CreateMutex(0,FALSE,0);
	_mutex1Aux[clientID]=CreateMutex(0,FALSE,0);
	_mutex2[clientID]=CreateMutex(0,FALSE,0);
	_mutex2Aux[clientID]=CreateMutex(0,FALSE,0);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_init(&_mutex1[clientID],0);
	pthread_mutex_init(&_mutex1Aux[clientID],0);
	pthread_mutex_init(&_mutex2[clientID],0);
	pthread_mutex_init(&_mutex2Aux[clientID],0);
#endif
	_mutex1LockLevel[clientID]=0;
	_mutex2LockLevel[clientID]=0;
}

simxVoid extApi_deleteMutexes(simxInt clientID)
{
#ifdef _WIN32
	CloseHandle(_mutex2Aux[clientID]);
	CloseHandle(_mutex2[clientID]);
	CloseHandle(_mutex1Aux[clientID]);
	CloseHandle(_mutex1[clientID]);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_destroy(&_mutex2Aux[clientID]);
	pthread_mutex_destroy(&_mutex2[clientID]);
	pthread_mutex_destroy(&_mutex1Aux[clientID]);
	pthread_mutex_destroy(&_mutex1[clientID]);
#endif
}

simxVoid _simpleLock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	while (WaitForSingleObject(mutex,INFINITE)!=WAIT_OBJECT_0)
		extApi_switchThread();
#elif defined (__linux) || defined (__APPLE__)
	while (pthread_mutex_lock(mutex)==-1)
		extApi_switchThread();
#endif
}

simxVoid _simpleUnlock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	ReleaseMutex(mutex);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_unlock(mutex);
#endif
}

simxVoid extApi_createGlobalMutex()
{
#ifdef _WIN32
	_globalMutex=CreateMutex(0,FALSE,0);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_init(&_globalMutex,0);
#endif
}

simxVoid extApi_deleteGlobalMutex()
{
#ifdef _WIN32
	CloseHandle(_globalMutex);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_destroy(&_globalMutex);
#endif
}

simxVoid extApi_globalSimpleLock()
{
#ifdef _WIN32
	_simpleLock(_globalMutex);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_globalMutex);
#endif
}

simxVoid extApi_globalSimpleUnlock()
{
#ifdef _WIN32
	_simpleUnlock(_globalMutex);
#elif defined (__linux) || defined (__APPLE__)
	_simpleUnlock(&_globalMutex);
#endif
}

simxVoid extApi_lockResources(simxInt clientID)
{
#ifdef _WIN32
	_simpleLock(_mutex1Aux[clientID]);
	if ( (GetCurrentThreadId()==_lock1ThreadId[clientID]) && (_mutex1LockLevel[clientID]>0) )
	{ // Already locked by this thread
		_mutex1LockLevel[clientID]++;
		_simpleUnlock(_mutex1Aux[clientID]);
		return;
	}
	// First level lock
	_simpleUnlock(_mutex1Aux[clientID]);
	_simpleLock(_mutex1[clientID]);
	_simpleLock(_mutex1Aux[clientID]);
	_lock1ThreadId[clientID]=GetCurrentThreadId();
	_mutex1LockLevel[clientID]=1;
	_simpleUnlock(_mutex1Aux[clientID]);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutex1Aux[clientID]);
	if ( (pthread_self()==_lock1ThreadId[clientID]) && (_mutex1LockLevel[clientID]>0) )
	{ // Already locked by this thread
		_mutex1LockLevel[clientID]++;
		_simpleUnlock(&_mutex1Aux[clientID]);
		return;
	}
	// First level lock
	_simpleUnlock(&_mutex1Aux[clientID]);
	_simpleLock(&_mutex1[clientID]);
	_simpleLock(&_mutex1Aux[clientID]);
	_lock1ThreadId[clientID]=pthread_self();
	_mutex1LockLevel[clientID]=1;
	_simpleUnlock(&_mutex1Aux[clientID]);
#endif
}

simxVoid extApi_unlockResources(simxInt clientID)
{
#ifdef _WIN32
	_simpleLock(_mutex1Aux[clientID]);
	_mutex1LockLevel[clientID]--;
	if (_mutex1LockLevel[clientID]==0)
	{
		_simpleUnlock(_mutex1Aux[clientID]);
		_simpleUnlock(_mutex1[clientID]);
	}
	else
		_simpleUnlock(_mutex1Aux[clientID]);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutex1Aux[clientID]);
	_mutex1LockLevel[clientID]--;
	if (_mutex1LockLevel[clientID]==0)
	{
		_simpleUnlock(&_mutex1Aux[clientID]);
		_simpleUnlock(&_mutex1[clientID]);
	}
	else
		_simpleUnlock(&_mutex1Aux[clientID]);
#endif
}

simxVoid extApi_lockSendStart(simxInt clientID)
{
#ifdef _WIN32
	_simpleLock(_mutex2Aux[clientID]);
	if ( (GetCurrentThreadId()==_lock2ThreadId[clientID]) && (_mutex2LockLevel[clientID]>0) )
	{ // Already locked by this thread
		_mutex2LockLevel[clientID]++;
		_simpleUnlock(_mutex2Aux[clientID]);
		return;
	}
	// First level lock
	_simpleUnlock(_mutex2Aux[clientID]);
	_simpleLock(_mutex2[clientID]);
	_simpleLock(_mutex2Aux[clientID]);
	_lock2ThreadId[clientID]=GetCurrentThreadId();
	_mutex2LockLevel[clientID]=1;
	_simpleUnlock(_mutex2Aux[clientID]);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutex2Aux[clientID]);
	if ( (pthread_self()==_lock2ThreadId[clientID]) && (_mutex2LockLevel[clientID]>0) )
	{ // Already locked by this thread
		_mutex2LockLevel[clientID]++;
		_simpleUnlock(&_mutex2Aux[clientID]);
		return;
	}
	// First level lock
	_simpleUnlock(&_mutex2Aux[clientID]);
	_simpleLock(&_mutex2[clientID]);
	_simpleLock(&_mutex2Aux[clientID]);
	_lock2ThreadId[clientID]=pthread_self();
	_mutex2LockLevel[clientID]=1;
	_simpleUnlock(&_mutex2Aux[clientID]);
#endif
}

simxVoid extApi_unlockSendStart(simxInt clientID)
{
#ifdef _WIN32
	_simpleLock(_mutex2Aux[clientID]);
	_mutex2LockLevel[clientID]--;
	if (_mutex2LockLevel[clientID]==0)
	{
		_simpleUnlock(_mutex2Aux[clientID]);
		_simpleUnlock(_mutex2[clientID]);
	}
	else
		_simpleUnlock(_mutex2Aux[clientID]);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutex2Aux[clientID]);
	_mutex2LockLevel[clientID]--;
	if (_mutex2LockLevel[clientID]==0)
	{
		_simpleUnlock(&_mutex2Aux[clientID]);
		_simpleUnlock(&_mutex2[clientID]);
	}
	else
		_simpleUnlock(&_mutex2Aux[clientID]);
#endif
}

simxChar extApi_launchThread(SIMX_THREAD_RET_TYPE(*startAddress)(simxVoid*))
{
#ifdef _WIN32
	return(_beginthread(startAddress,0,0)!=0);
#elif defined (__linux) || defined (__APPLE__)
	pthread_t th;
	return (pthread_create(&th,NULL,startAddress,NULL) == 0);
#endif
}

simxChar extApi_connectToServer_socket(simxInt clientID,const simxChar* theConnectionAddress,simxInt theConnectionPort)
{ /* return 1: success */
	struct hostent *hp;
	simxUInt addr;
#ifdef _WIN32
	if (WSAStartup(0x101,&_socketWsaData)!=0)
		return(0);
#endif
	_socketConn[clientID]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(_socketConn[clientID]==INVALID_SOCKET)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		return(0);
	}
	if (inet_addr(theConnectionAddress)==INADDR_NONE)
		hp=gethostbyname(theConnectionAddress);
	else
	{
		addr=inet_addr(theConnectionAddress);
		hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
	}
	if(hp==NULL)
	{
#ifdef _WIN32
		closesocket(_socketConn[clientID]);
		WSACleanup();
#elif defined (__linux) || defined (__APPLE__)
		close(_socketConn[clientID]);
#endif
		return(0);
	}
	_socketServer[clientID].sin_addr.s_addr=0;
    memcpy(&_socketServer[clientID].sin_addr.s_addr, hp->h_addr, hp->h_length);
	_socketServer[clientID].sin_family=AF_INET;
	_socketServer[clientID].sin_port=htons(theConnectionPort);
	if(connect(_socketConn[clientID],(struct sockaddr*)&_socketServer[clientID],sizeof(_socketServer[clientID])))
	{
#ifdef _WIN32
		closesocket(_socketConn[clientID]);
		WSACleanup();
#elif defined (__linux) || defined (__APPLE__)
		close(_socketConn[clientID]);
#endif
		return(0);
	}
	return(1);
}

simxVoid extApi_cleanUp_socket(simxInt clientID)
{
#ifdef _WIN32
	closesocket(_socketConn[clientID]);
	WSACleanup();
#elif defined (__linux) || defined (__APPLE__)
	close(_socketConn[clientID]);
#endif
}

simxInt extApi_send_socket(simxInt clientID,const simxChar* data,simxInt dataLength)
{
	return(send(_socketConn[clientID],data,dataLength,0));
}

simxInt extApi_recv_socket(simxInt clientID,simxChar* data,simxInt maxDataLength)
{
	return(recv(_socketConn[clientID],data,maxDataLength,0));
}

