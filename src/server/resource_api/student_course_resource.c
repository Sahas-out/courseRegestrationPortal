#include "resource_api.h"
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define MAX_RECORDS 1000

char student_course_keys[MAX_RECORDS][NAME_SIZE * 2]; // Concatenated student_name and course_name
int n_student_course_keys = 0;
pthread_mutex_t mutex_stu_course_keys;
pthread_mutex_t mutex_stu_course_record[MAX_RECORDS];

void intialize_stu_course_record(struct Student_course stu_course){
  pthread_mutex_lock(&mutex_stu_course_keys);
  char key[NAME_SIZE * 2];
  snprintf(key, sizeof(key), "%s|%s", stu_course.student_name, stu_course.course_name);
  strcpy(student_course_keys[n_student_course_keys], key);
  pthread_mutex_init(&mutex_stu_course_record[n_student_course_keys],NULL);
  n_student_course_keys++;
  pthread_mutex_unlock(&mutex_stu_course_keys);
}

  void intialize_student_course_resources() {
    int fd = open(std_course_path, O_RDONLY);
    struct Student_course stu_course;
    pthread_mutex_init(&mutex_stu_course_keys, NULL);
    while ((read(fd, &stu_course, sizeof(struct Student_course)) > 0)) {
      intialize_stu_course_record(stu_course);
    }
    close(fd);
  }

  int get_student_course_key(char student_name[NAME_SIZE], char course_name[NAME_SIZE]) {
    pthread_mutex_lock(&mutex_stu_course_keys);
    char key[NAME_SIZE * 2];
    snprintf(key, sizeof(key), "%s|%s", student_name, course_name);
    for (int i = 0; i < n_student_course_keys; i++) {
      if (strcmp(student_course_keys[i], key) == 0) {
        pthread_mutex_unlock(&mutex_stu_course_keys);
        return i;
      }
    }
    pthread_mutex_unlock(&mutex_stu_course_keys);
    return -1;
  }

  int add_student_course(struct Student_course stu_course) {
    int fd;
    fd = open(std_course_path, O_WRONLY);
    int rec_no = get_student_course_key(stu_course.student_name, stu_course.course_name);
    if(rec_no != -1) return error_duplicate_record;
    stu_course.denrolled = 0;
    lseek(fd, 0, SEEK_END);
    write(fd, &stu_course, sizeof(struct Student_course));
    intialize_stu_course_record(stu_course);
    close(fd);
    return 0;
  }

  struct Array_student_course get_student_course(char student_name[NAME_SIZE]) {
    int fd;
    struct Array_student_course matching_courses;
    int count = 0;

    fd = open(std_course_path, O_RDONLY);
    int n_rec = n_student_course_keys;
    for(int i=0; i<n_rec; i++)
      pthread_mutex_lock(&mutex_stu_course_record[i]);

    struct Student_course stu_course;
    while (read(fd, &stu_course, sizeof(struct Student_course)) > 0) {
      if (strcmp(stu_course.student_name, student_name) == 0) {
        matching_courses.arr[count++] = stu_course;
      }
    }
    matching_courses.n = count;
    for(int i=0; i<n_rec; i++)
      pthread_mutex_unlock(&mutex_stu_course_record[i]);
    close(fd);

    return matching_courses;
  }

  int denroll_student_course(struct Student_course stu_course) {
    struct flock lock;
    int fd;

    fd = open(std_course_path, O_WRONLY);
    int rec_no = get_student_course_key(stu_course.student_name, stu_course.course_name);
    if(rec_no == -1) return error_record_nonexistent;
    pthread_mutex_lock(&mutex_stu_course_record[rec_no]);
    stu_course.denrolled = 1;
    lseek(fd,rec_no * sizeof(struct Student_course), SEEK_SET);
    write(fd, &stu_course, sizeof(struct Student_course));
    pthread_mutex_unlock(&mutex_stu_course_record[rec_no]);
    close(fd);
    return 0;
  }
