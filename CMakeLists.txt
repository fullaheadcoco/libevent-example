cmake_minimum_required(VERSION 3.10)
project(libevent_example C)

set(CMAKE_C_STANDARD 99)

# libevent를 서브디렉토리로 추가
# 서브모듈 디렉토리 추가 (libs/my_static_lib 경로에 서브모듈이 있고, 그 안에 CMakeLists.txt가 있다고 가정)
add_subdirectory(external/libevent)

# 서브모듈의 헤더 파일 경로 포함
# 만약 서브모듈의 CMakeLists.txt에서 target_include_directories(... PUBLIC ...) 등으로
# 헤더 경로를 제대로 export하지 않았다면, 여기서 직접 지정
# event-config.h 가 생성됨
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/libevent/include)
# server.c, client.c 파일에서 libevent 헤더 파일을 사용
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/libevent/include)

# 서버 빌드
add_executable(server src/server.c)
# 서브모듈에서 정의한 정적 라이브러리 타겟을 주 프로젝트 타겟에 링킹
# 이 때, 라이브러리 파일(.a 또는 .lib) 경로가 아닌 CMake 타겟 이름을 사용
target_link_libraries(server event_static)

# 클라이언트 빌드
add_executable(client src/client.c)
target_link_libraries(client event_static)

# hello_world.c
add_executable(hello_world src/hello_world.c)
target_link_libraries(hello_world event_shared)
