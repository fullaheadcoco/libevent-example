#include <arpa/inet.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void read_cb(struct bufferevent *bev, void *ctx) {
  struct evbuffer *input = bufferevent_get_input(bev);
  size_t len = evbuffer_get_length(input);
  if (len > 0) {
    char *data = malloc(len + 1);
    evbuffer_remove(input, data, len);
    data[len] = '\0';  // 문자열 종료 문자 추가
    printf("클라이언트: 서버로부터 받은 응답 - %s\n", data);
    free(data);
  }
}

static void event_cb(struct bufferevent *bev, short events, void *ctx) {
  if (events & BEV_EVENT_CONNECTED) {
    printf("서버에 연결되었습니다.\n");
    char message[] = "안녕하세요, 서버!";
    bufferevent_write(bev, message, strlen(message));
  } else if (events & BEV_EVENT_ERROR) {
    int err = bufferevent_socket_get_dns_error(bev);
    if (err) {
      printf("DNS 오류: %s\n", evutil_gai_strerror(err));
    } else {
      perror("버퍼 이벤트 에러");
    }
    bufferevent_free(bev);
    event_base_loopbreak(ctx);
  } else if (events & BEV_EVENT_EOF) {
    printf("서버와 연결 종료\n");
    bufferevent_free(bev);
    event_base_loopbreak(ctx);
  } else if (events & BEV_EVENT_TIMEOUT) {
    printf("타임아웃 발생\n");
    bufferevent_free(bev);
    event_base_loopbreak(ctx);
  }
}

int main() {
  struct event_base *base = event_base_new();
  struct sockaddr_in sin = {0};
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  sin.sin_port = htons(9876);

  struct bufferevent *bev =
      bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_cb, NULL, event_cb, base);
  bufferevent_enable(bev, EV_READ | EV_WRITE);

  // 타임아웃 설정 (선택사항)
  struct timeval read_tv = {10, 0};   // 10초 읽기 타임아웃
  struct timeval write_tv = {10, 0};  // 10초 쓰기 타임아웃
  bufferevent_set_timeouts(bev, &read_tv, &write_tv);

  if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) <
      0) {
    perror("연결 실패");
    bufferevent_free(bev);
    event_base_free(base);
    return -1;
  }

  // 메시지는 연결 성공 이벤트에서 보냄

  event_base_dispatch(base);

  event_base_free(base);
  return 0;
}