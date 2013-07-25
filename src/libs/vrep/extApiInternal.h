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

#ifndef __EXTAPIINTERNAL_
#define __EXTAPIINTERNAL_

#include "extApiPlatform.h"

#define SIMX_INIT_BUFF_SIZE 500
#define SIMX_MIN_BUFF_INCR 500

#define _REPLY_WAIT_TIMEOUT_IN_MS 5000
#define _MIN_SPLIT_AMOUNT_IN_BYTES 100

/* Out buffer for messages */
simxChar* _messageToSend[MAX_EXT_API_CONNECTIONS];
simxInt _messageToSend_bufferSize[MAX_EXT_API_CONNECTIONS];
simxInt _messageToSend_dataSize[MAX_EXT_API_CONNECTIONS];

/* Temp buffer for split commands to send */
simxChar* _splitCommandsToSend[MAX_EXT_API_CONNECTIONS];
simxInt _splitCommandsToSend_bufferSize[MAX_EXT_API_CONNECTIONS];
simxInt _splitCommandsToSend_dataSize[MAX_EXT_API_CONNECTIONS];

/* In buffer for messages */
simxChar* _messageReceived[MAX_EXT_API_CONNECTIONS];
simxInt _messageReceived_bufferSize[MAX_EXT_API_CONNECTIONS];
simxInt _messageReceived_dataSize[MAX_EXT_API_CONNECTIONS];

/* Temp buffer for split commands received */
simxChar* _splitCommandsReceived[MAX_EXT_API_CONNECTIONS];
simxInt _splitCommandsReceived_bufferSize[MAX_EXT_API_CONNECTIONS];
simxInt _splitCommandsReceived_dataSize[MAX_EXT_API_CONNECTIONS];

/* Temp buffer for last fetched command */
simxChar* _commandReceived[MAX_EXT_API_CONNECTIONS];
simxInt _commandReceived_bufferSize[MAX_EXT_API_CONNECTIONS];
simxInt _commandReceived_simulationTime[MAX_EXT_API_CONNECTIONS];

/* Other variables */
simxInt _nextConnectionID[MAX_EXT_API_CONNECTIONS];
simxInt _replyWaitTimeoutInMs[MAX_EXT_API_CONNECTIONS];

simxInt _minCommunicationDelay[MAX_EXT_API_CONNECTIONS];
simxChar _communicationThreadRunning[MAX_EXT_API_CONNECTIONS];
simxInt _nextMessageIDToSend[MAX_EXT_API_CONNECTIONS];
simxInt _waitBeforeSendingAgainWhenMessageIDArrived[MAX_EXT_API_CONNECTIONS];
simxInt _lastReceivedMessageID[MAX_EXT_API_CONNECTIONS];
simxInt _connectionID[MAX_EXT_API_CONNECTIONS];
const simxChar* _tempConnectionAddress[MAX_EXT_API_CONNECTIONS];
simxInt _tempConnectionPort[MAX_EXT_API_CONNECTIONS];
simxChar _tempDoNotReconnectOnceDisconnected[MAX_EXT_API_CONNECTIONS];

#endif /* __EXTAPIINTERNAL_ */
