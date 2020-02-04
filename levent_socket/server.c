#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

void read_cb(struct bufferevent *bev,void *arg);

void write_cb(struct bufferevent *bev,void *arg);

void event_cb(struct bufferevent *bev,short events,void *arg);

void send_cb(evutil_socket_t fd,short what,void *arg);

void cb_listener(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *addr, int len,void *ptr);

int main(int argc, char *argv[])
{
	struct sockaddr_in serv;
	memset(&serv,0,sizeof(serv));
	serv.sin_family=AF_INET;
	serv.sin_port=htons(9876);
	serv.sin_addr.s_addr=htonl(INADDR_ANY);

	struct event_base* base=event_base_new();

	struct evconnlistener* listener;

	listener=evconnlistener_new_bind(base,cb_listener,base,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,36,(struct sockaddr*)&serv,sizeof(serv));
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	return 0;

}

void read_cb(struct bufferevent *bev,void *arg)
{
	char buf[1024]={0};
	bufferevent_read(bev,buf,sizeof(buf));
	char *p="I hava recieved your data!";
	printf("client say:%s\n",buf);
}

void write_cb(struct bufferevent *bev,void *arg)
{
	printf("I am callbacked function...you have send\n");
}

void event_cb(struct bufferevent *bev,short events,void *arg)
{
	if(events & BEV_EVENT_EOF)
	{
		printf("connection closed\n");
	}else if(events & BEV_EVENT_ERROR)
	{
		printf("some other error\n");
	}

	bufferevent_free(bev);
	printf("buffer has been free\n");
}

void cb_listener(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *addr, int len,void *ptr)
{
	printf("connect new client\n");

	struct event_base* base=(struct event_base*)ptr;
	struct bufferevent *bev=NULL;
	bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
	bufferevent_enable(bev,EV_READ);
}

void send_cb(evutil_socket_t fd,short what,void *arg)
{
	char buf[1024]={0};
	struct bufferevent* bev=(struct bufferevent*)arg;
	read(fd,buf,sizeof(buf));
	bufferevent_write(bev,buf,strlen(buf)+1);
}