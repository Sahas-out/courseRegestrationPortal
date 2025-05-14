#include "client_routine.h"

#include "server.h"
#include "pthread.h"
#include "resource_api.h"
#include <string.h>
#include <stdio.h>
extern struct Server_utils server_util;

int authenticate_user(struct Authentication_token client_creds){
  struct Authentication_token auth_tok;
  auth_tok.role = client_creds.role;
  if (auth_tok.role == STUDENT) {
    struct Student stu = get_student(client_creds.user_name);
    strcpy(auth_tok.user_name, stu.student_name);
    strcpy(auth_tok.pass, stu.user_password);
  } 
  else if (auth_tok.role == FACULTY) {
    struct Faculty fac = get_faculty(client_creds.user_name);
    strcpy(auth_tok.user_name, fac.faculty_name);
    strcpy(auth_tok.pass, fac.user_password);
  } 
  else {
    strcpy(auth_tok.user_name, ADMIN_USER);
    strcpy(auth_tok.pass, ADMIN_PASS);
  }

  if (strcmp(auth_tok.user_name, client_creds.user_name) == 0 &&
    strcmp(auth_tok.pass, client_creds.pass) == 0 &&
    auth_tok.role == client_creds.role) {
    return 1;
  }
  return 0;
}
void handle_not_priviliged(int fd){
  send(fd,&(struct Message){NOT_PERMITTED,"you are not priviliged to do this action"},sizeof(struct Message),0);
}
void handle_resource_error(int error_code,int client_fd){
  if(error_code == error_duplicate_record)
    send(client_fd,&(struct Message){REQ_FAIL,"duplicate record has been found"},sizeof(struct Message),0);
  else if(error_code == error_record_nonexistent)
    send(client_fd,&(struct Message){REQ_FAIL,"no such record exist"},sizeof(struct Message),0);
}
void * client_routine(void* arg){
  struct Client* client = (struct Client*) arg;
  
  recv(client->fd,&client->creds,sizeof(struct Authentication_token),0);
  int authenticated = authenticate_user(client->creds); 
  if(authenticated) send(client->fd,&(struct Message){REQ_SUCCESS,"Succesfully authenticated"},sizeof(struct Message),0);
  else send(client->fd,&(struct Message){REQ_FAIL,"Authentication failure"},sizeof(struct Message),0);
  while(client->last_msg.req_kind != LOGOUT && authenticated){

    recv(client->fd,&client->last_msg,sizeof(struct Message),0);

    switch ((client->last_msg).req_kind){

      case ADD_STUDENT:{
        if(client->creds.role != ADMIN){ handle_not_priviliged(client->fd); continue;}
        struct Student stu;
        memcpy(&stu,client->last_msg.msg,sizeof(struct Student));
        int r = add_student(stu);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd,&(struct Message){REQ_SUCCESS,"succesfully added student"},sizeof(struct Message),0);
        break;
      }

      case GET_STUDENT:{
        char stu_name[NAME_SIZE];
        strcpy(stu_name,client->last_msg.msg);
        if(client->creds.role != ADMIN && (client->creds.role != STUDENT || strcmp(client->creds.user_name,stu_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        struct Student stu = get_student(stu_name);
        if(stu.active == error_record_nonexistent){handle_resource_error(stu.active,client->fd); continue;}
        struct Message msg_to_send;
        msg_to_send.req_kind = REQ_SUCCESS;
        memcpy(msg_to_send.msg,&stu,sizeof(struct Student));
        send(client->fd,&msg_to_send,sizeof(struct Message),0);
        break;
      }


      case UPDATE_STUDENT:{
        struct Student student;
        memcpy(&student,client->last_msg.msg,sizeof(struct Student));        
        if(client->creds.role != ADMIN && (client->creds.role != STUDENT || strcmp(client->creds.user_name,student.student_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r = update_student(student);        
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd,&(struct Message){REQ_SUCCESS,"succesfully updated the student details"},sizeof(struct Message),0);
        break;
      }

      case ADD_FACULTY: {
        if(client->creds.role != ADMIN){handle_not_priviliged(client->fd); continue;}
        struct Faculty fac;
        memcpy(&fac, client->last_msg.msg, sizeof(struct Faculty));
        int r = add_faculty(fac);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully added faculty"}, sizeof(struct Message), 0);
        break;
      }

      case GET_FACULTY: {
        char fac_name[NAME_SIZE];
        strcpy(fac_name, client->last_msg.msg);        
        if(client->creds.role != ADMIN && (client->creds.role != FACULTY || strcmp(client->creds.user_name,fac_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        struct Faculty fac = get_faculty(fac_name);
        if(fac.active == error_record_nonexistent){handle_resource_error(fac.active,client->fd); continue;}
        struct Message msg_to_send;
        msg_to_send.req_kind = REQ_SUCCESS;
        memcpy(msg_to_send.msg, &fac, sizeof(struct Faculty));
        send(client->fd, &msg_to_send, sizeof(struct Message), 0);
        break;
      }

      case UPDATE_FACULTY: {
        struct Faculty faculty;
        memcpy(&faculty, client->last_msg.msg, sizeof(struct Faculty));        
        if(client->creds.role != ADMIN && (client->creds.role != FACULTY || strcmp(client->creds.user_name,faculty.faculty_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r = update_faculty(faculty);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully updated the faculty details"}, sizeof(struct Message), 0);
        break;
      }

      case ADD_STUDENT_COURSE: {
        struct Student_course stu_course;
        memcpy(&stu_course, client->last_msg.msg, sizeof(struct Student_course));
        if(client->creds.role != ADMIN && (client->creds.role != STUDENT || strcmp(client->creds.user_name,stu_course.student_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r = add_student_course(stu_course);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully added student course"}, sizeof(struct Message), 0);
        break;
      }

      case GET_STUDENT_COURSE: {
        char student_name[NAME_SIZE];
        strcpy(student_name, client->last_msg.msg);
        if(client->creds.role != ADMIN && (client->creds.role != STUDENT || strcmp(client->creds.user_name,student_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        struct Array_student_course matching_courses = get_student_course(student_name);
        struct Message msg_to_send;
        msg_to_send.req_kind = REQ_SUCCESS;
        memcpy(msg_to_send.msg, &matching_courses, sizeof(struct Array_student_course));
        send(client->fd, &msg_to_send, sizeof(struct Message), 0);
        break;
      }

      case DENROLL_STUDENT_COURSE: {
        struct Student_course stu_course;
        memcpy(&stu_course, client->last_msg.msg, sizeof(struct Student_course));
        if(client->creds.role != ADMIN && (client->creds.role != STUDENT || strcmp(client->creds.user_name,stu_course.student_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r = denroll_student_course(stu_course);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully denrolled student from course"}, sizeof(struct Message), 0);
        break;
      }

      case ADD_COURSE: {
        struct Course course;
        memcpy(&course, client->last_msg.msg, sizeof(struct Course));        
        if(client->creds.role != ADMIN && (client->creds.role != FACULTY || strcmp(client->creds.user_name,course.facuilty_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r =add_course(course);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully added course"}, sizeof(struct Message), 0);
        break;
      }

      case GET_FACULTY_COURSE: {
        char faculty_name[NAME_SIZE];
        strcpy(faculty_name, client->last_msg.msg);
        if(client->creds.role != ADMIN && (client->creds.role != FACULTY || strcmp(client->creds.user_name,faculty_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        struct Array_faculty_course matching_courses = get_faculty_courses(faculty_name);
        struct Message msg_to_send;
        msg_to_send.req_kind = REQ_SUCCESS;
        memcpy(msg_to_send.msg, &matching_courses, sizeof(struct Array_faculty_course));
        send(client->fd, &msg_to_send, sizeof(struct Message), 0);
        break;
      }

      case REMOVE_COURSE: {
        struct Course course;
        memcpy(&course, client->last_msg.msg, sizeof(struct Course));
        if(client->creds.role != ADMIN && (client->creds.role != FACULTY || strcmp(client->creds.user_name,course.facuilty_name) !=0)){
          handle_not_priviliged(client->fd); continue;
        }
        int r = remove_course(course);
        if(r != 0){handle_resource_error(r,client->fd); continue;}
        send(client->fd, &(struct Message){REQ_SUCCESS, "Successfully removed course"}, sizeof(struct Message), 0);
        break;
      }

      case LOGOUT:
        send(client->fd,&(struct Message){REQ_SUCCESS,"succesfully logged out"},sizeof(struct Message),0);
        break;
      default:
        send(client->fd,&(struct Message){VOID,"invalid request sent\n"},sizeof(struct Message),0);
        break;
    }
  }

  close(client->fd);
  client->last_msg.req_kind = VOID;
  pthread_mutex_lock(&server_util.mutex_con_info);
  server_util.n_free_thread++;
  client->status = FREE;
  pthread_mutex_unlock(&server_util.mutex_con_info);
  return NULL; 
}

