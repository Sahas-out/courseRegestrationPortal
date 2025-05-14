
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "client_ui.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
  if(argc != 2){
    printf("usage %s <role>(0: Admin 1:Student 2:Faculty)\n",argv[0]);
    return 0;
  }
  enum Role client_role = atoi(argv[1]);
  int server_sock;
  struct sockaddr_in server_addr;
  if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) return -1;

  struct Message last_msg_rcv;
  
  recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
  printf("%s\n",last_msg_rcv.msg);
  if (last_msg_rcv.req_kind != REQ_SUCCESS) {close(server_sock); return 0;}
  
  struct Authentication_token auth_token;
  auth_token.role = client_role;
  get_user_credentials(&auth_token);
  send(server_sock,&auth_token,sizeof(struct Authentication_token),0);
  recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
  printf("%s\n",last_msg_rcv.msg);
  if(last_msg_rcv.req_kind != REQ_SUCCESS) {close(server_sock); return 0;}
  

  enum Req_kind usr_req = VOID;

  while (usr_req != LOGOUT)
  {
    display_help_menu(client_role);
    usr_req = get_user_request(); 
    switch (usr_req)
    {

    case ADD_STUDENT:
    {
      struct Student new_stu = add_student();
      struct Message msg_to_send;
      msg_to_send.req_kind = ADD_STUDENT;
      memcpy(msg_to_send.msg, &new_stu, sizeof(struct Student));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }
    case DISPLAY_STUDENT:
    {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_STUDENT;
      get_student_name(msg_to_send.msg);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Student stu;
      memcpy(&stu, last_msg_rcv.msg, sizeof(struct Student));
      display_student(stu);
      break;
    }
    case UPDATE_STUDENT:
    {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_STUDENT;
      get_student_name(msg_to_send.msg);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Student stu;
      memcpy(&stu, last_msg_rcv.msg, sizeof(struct Student));
      update_student(&stu);
      msg_to_send.req_kind = UPDATE_STUDENT;
      memcpy(msg_to_send.msg, &stu, sizeof(struct Student));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }
    case ADD_FACULTY:
    {
      struct Faculty new_fac = add_faculty();
      struct Message msg_to_send;
      msg_to_send.req_kind = ADD_FACULTY;
      memcpy(msg_to_send.msg, &new_fac, sizeof(struct Faculty));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case DISPLAY_FACULTY:
    {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_FACULTY;
      get_faculty_name(msg_to_send.msg);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Faculty fac;
      memcpy(&fac, last_msg_rcv.msg, sizeof(struct Faculty));
      display_faculty(fac);
      break;
    }

    case UPDATE_FACULTY:
    {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_FACULTY;
      get_faculty_name(msg_to_send.msg);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Faculty fac;
      memcpy(&fac, last_msg_rcv.msg, sizeof(struct Faculty));
      update_faculty(&fac);
      msg_to_send.req_kind = UPDATE_FACULTY;
      memcpy(msg_to_send.msg, &fac, sizeof(struct Faculty));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case ADD_STUDENT_COURSE: {
      struct Student_course new_stu_course = get_student_course();
      struct Message msg_to_send;
      msg_to_send.req_kind = ADD_STUDENT_COURSE;
      memcpy(msg_to_send.msg, &new_stu_course, sizeof(struct Student_course));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case GET_STUDENT_COURSE: {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_STUDENT_COURSE;
      get_student_name(msg_to_send.msg);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Array_student_course courses;
      memcpy(&courses, last_msg_rcv.msg, sizeof(struct Array_student_course));
      display_student_course(courses,msg_to_send.msg);
      break;
    }

    case DENROLL_STUDENT_COURSE: {
      struct Student_course stu_course = get_student_course();
      struct Message msg_to_send;
      msg_to_send.req_kind = DENROLL_STUDENT_COURSE;
      memcpy(msg_to_send.msg, &stu_course, sizeof(struct Student_course));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case ADD_COURSE: {
      struct Course new_course = get_course();
      struct Message msg_to_send;
      msg_to_send.req_kind = ADD_COURSE;
      memcpy(msg_to_send.msg, &new_course, sizeof(struct Course));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case GET_FACULTY_COURSE: {
      struct Message msg_to_send;
      msg_to_send.req_kind = GET_FACULTY_COURSE;
      char faculty_name[NAME_SIZE];
      get_faculty_name(faculty_name);
      strcpy(msg_to_send.msg, faculty_name);
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      struct Array_faculty_course courses;
      memcpy(&courses, last_msg_rcv.msg, sizeof(struct Array_faculty_course));
      display_fac_courses(courses, faculty_name);
      break;
    }

    case REMOVE_COURSE: {
      struct Course course_to_remove = get_course();
      struct Message msg_to_send;
      msg_to_send.req_kind = REMOVE_COURSE;
      memcpy(msg_to_send.msg, &course_to_remove, sizeof(struct Course));
      send(server_sock, &msg_to_send, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;
    }

    case LOGOUT:
      send(server_sock, &(struct Message){LOGOUT, "logout"}, sizeof(struct Message), 0);
      recv(server_sock, &last_msg_rcv, sizeof(struct Message), 0);
      printf("%s\n", last_msg_rcv.msg);
      break;

    default:
      break;
    }
  }
  close(server_sock);
}
