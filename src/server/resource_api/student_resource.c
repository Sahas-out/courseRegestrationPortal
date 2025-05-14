#include "resource_api.h"
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define MAX_RECORDS 1000

char student_keys[MAX_RECORDS][NAME_SIZE];
int n_student_keys =0;
pthread_mutex_t mutex_stu_keys;
pthread_mutex_t mutex_stu_record[MAX_RECORDS];

void intialize_student_record(char stu_name[NAME_SIZE]){
  pthread_mutex_lock(&mutex_stu_keys);
  strcpy(student_keys[n_student_keys], stu_name);
  pthread_mutex_init(&mutex_stu_record[n_student_keys], NULL);
  n_student_keys++;
  pthread_mutex_unlock(&mutex_stu_keys);
}
void intialize_student_resources(){
  int fd = open(student_path,O_RDONLY);
  struct Student stu;
  pthread_mutex_init(&mutex_stu_keys, NULL);
  while((read(fd, &stu, sizeof(struct Student)) > 0)){
    intialize_student_record(stu.student_name);
  }
  close(fd);
}
int get_student_key(char match_value[NAME_SIZE]){
  pthread_mutex_lock(&mutex_stu_keys);
  for(int i =0; i<n_student_keys; i++){
    if(strcmp(student_keys[i],match_value) == 0){
      pthread_mutex_unlock(&mutex_stu_keys);
      return i;
    }
  }
  pthread_mutex_unlock(&mutex_stu_keys);
  return -1;
}

int add_student(struct Student student){ 
  int fd;
  fd = open(student_path,O_WRONLY);
  int rec_no = get_student_key(student.student_name);
  if(rec_no != -1) return error_duplicate_record; 
  student.active = 1;
  lseek(fd, 0, SEEK_END);
  write(fd,&student,sizeof(struct Student));
  intialize_student_record(student.student_name);
  close(fd);
  return 0;
}

int update_student(struct Student student){
  int fd;
  fd = open(student_path,O_WRONLY);
  int rec_no = get_student_key(student.student_name);
  if(rec_no == -1) return error_record_nonexistent; 
  pthread_mutex_lock(&mutex_stu_record[rec_no]);
  lseek(fd,rec_no*sizeof(struct Student),SEEK_SET);
  write(fd, &student, sizeof(struct Student));
  // printf("Enter char to realease lock\n");
  // getchar();
  pthread_mutex_unlock(&mutex_stu_record[rec_no]);
  close(fd);
  return 0;
}

struct Student get_student(char student_name[NAME_SIZE]){
  int fd;
  fd = open(student_path,O_RDONLY);
  int rec_no = get_student_key(student_name);
  if(rec_no == -1) return (struct Student){"",0.0,"",error_record_nonexistent}; 
  pthread_mutex_lock(&mutex_stu_record[rec_no]);
  lseek(fd,rec_no*sizeof(struct Student),SEEK_SET);
  struct Student student;
  read(fd,&student,sizeof(struct Student));
  pthread_mutex_unlock(&mutex_stu_record[rec_no]);
  close(fd);
  return student;
}



