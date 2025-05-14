#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define SERVER_PORT 9999
#define BIND_ERROR 101
#define LISTEN_ERROR 102
#define BACKLOG_SIZE 5
#define MAX_CONNECTIONS 20
#define MAX_MSG_SIZE 1000
#define LOGIN_TRY_LIMIT 3
#define NAME_SIZE 30
enum Req_kind
{
    SERVER_FULL = 1,
    LOGOUT = 2,
    REQ_SUCCESS = 3,
    REQ_FAIL = 4,
    NOT_PERMITTED = 5,
    VOID = 9,

    ADD_STUDENT = 11,
    UPDATE_STUDENT = 12,
    GET_STUDENT = 13,
    DISPLAY_STUDENT = 14,

    ADD_FACULTY = 21,
    UPDATE_FACULTY = 22,
    GET_FACULTY = 23,
    DISPLAY_FACULTY = 24,

    ADD_STUDENT_COURSE = 41,
    GET_STUDENT_COURSE = 42,
    DENROLL_STUDENT_COURSE = 43,

    ADD_COURSE = 31,
    REMOVE_COURSE = 32,
    GET_FACULTY_COURSE = 33
};
enum Role
{
  ADMIN = 0,
  STUDENT = 1,
  FACULTY = 2
};
enum Status
{
  OPENED = 0,
  CLOSED = 1,
  FREE = 2
};
struct Authentication_token
{
  enum Role role;
  char user_name[NAME_SIZE];
  char pass[NAME_SIZE];
};
struct Message
{
  enum Req_kind req_kind;
  char msg[MAX_MSG_SIZE];
};
struct Client
{
  enum Status status;
  struct Message last_msg;
  struct Authentication_token creds;
  pthread_t thread;
  int fd;
};
struct Server_utils
{
  struct Client clients[MAX_CONNECTIONS];
  int n_free_thread;
  pthread_mutex_t mutex_con_info;
  int client_ids[MAX_CONNECTIONS];
  int fd;
  struct sockaddr_in addr;
};
