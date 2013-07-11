#ifndef FTP_HANDLERS_H
#define FTP_HANDLERS_H

#include "ftp_server.h"

void user_handler(struct session_data*);
void port_handler(struct session_data*);
void pasv_handler(struct session_data*);
void size_handler(struct session_data*);
void type_handler(struct session_data*);
void retr_handler(struct session_data*);
void quit_handler(struct session_data*);

void empty_handler(struct session_data*);

#endif
