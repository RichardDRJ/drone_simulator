#ifndef FTP_MESSAGES_H
#define FTP_MESSAGES_H

#define MSG_OPERATION_SUCCESS "220 operation successful\r\n"
#define MSG_COMMAND_OK "200 command ok\r\n"
#define MSG_UNSUPPORTED "202 command unsupported\r\n"
#define MSG_LOGIN_SUCCESS "230 login successful\r\n"
#define MSG_PASSIVE_SUCCESS "227 PASV ok"
#define MSG_RETR_SUCCESS "226 operation successful\r\n"
#define MSG_TELL_SIZE  "213"
#define MSG_OPENING_BINARY_CONN "150 opening binary connection\r\n"
#define MSG_QUIT_SUCCESS "221 quit successful\r\n"
#define MSG_NO_SUCH_FILE "550 file not found\r\n"

#endif
