#include <arpa/inet.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void echo_read_cb(struct bufferevent *bev, void *ctx) {
  struct evbuffer *input = bufferevent_get_input(bev);
  struct evbuffer *output = bufferevent_get_output(bev);

  size_t len = evbuffer_get_length(input);
  char *data = malloc(len);
  evbuffer_remove(input, data, len);

  printf("서버: 받은 데이터 - %s\n", data);
  evbuffer_add(output, data, len);

  free(data);
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx) {
  if (events & BEV_EVENT_ERROR) perror("버퍼 이벤트 에러");
  if (events & BEV_EVENT_EOF) printf("연결 종료\n");
  bufferevent_free(bev);
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                           struct sockaddr *address, int socklen, void *ctx) {
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *bev =
      bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main() {
  struct event_base *base = event_base_new();
  struct sockaddr_in sin = {0};
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(9876);

  struct evconnlistener *listener = evconnlistener_new_bind(
      base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
      (struct sockaddr *)&sin, sizeof(sin));

  event_base_dispatch(base);

  evconnlistener_free(listener);
  event_base_free(base);

  return 0;
}