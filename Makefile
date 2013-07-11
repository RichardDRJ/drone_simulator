all:
	gcc -o server.a main.c ftp_server.c error.c trie.c ftp_handlers.c -lpthread
