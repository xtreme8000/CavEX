#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

struct server_interface {
	void (*player_pos)(void* user, double x, double y, double z);
	void* user;
};

#endif
