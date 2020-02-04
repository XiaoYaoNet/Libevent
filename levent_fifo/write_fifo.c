#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <event2/event.h>

void write_cd(evutil_socket_t fd,short what,void *arg);

int main(int argc, const char* argv[])
{
	int fd=open("myfifo",O_WRONLY|O_NONBLOCK);
	if(fd==1)
	{
		perror("open error");
		exit(1);
	}

	struct event_base* base=event_base_new();

	struct event_base* ev=event_new(base,fd,EV_WRITE,write_cd,NULL);

	event_add(ev,NULL);

	event_base_dispatch(base);

	event_free(ev);
	event_base_free(base);
	close(fd);

	return 0;
}

void write_cd(evutil_socket_t fd,short what,void *arg)
{
	char buf[1024]={0};
	static int num=666;
	sprintf(buf,"hello,world==%d\n",num);
	write(fd,buf,strlen(buf)+1);
}