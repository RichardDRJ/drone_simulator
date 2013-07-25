#ifndef FTP_HANDLERS_H
#define FTP_HANDLERS_H

#include "ftp_server.h"

void ftp_user_handler(void*);
void ftp_port_handler(void*);
void ftp_pasv_handler(void*);
void ftp_size_handler(void*);
void ftp_type_handler(void*);
void ftp_retr_handler(void*);
void ftp_quit_handler(void*);

void ftp_empty_handler(void*);

#endif
