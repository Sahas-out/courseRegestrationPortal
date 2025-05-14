#include "resource_api.h"
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define MAX_RECORDS 1000

char course_keys[MAX_RECORDS][NAME_SIZE]; // Array to store course names
int n_course_keys = 0;
pthread_mutex_t mutex_course_keys;

void intialize_course_resources() {
    int fd = open(course_path, O_RDONLY);
    struct Course course;
    while ((read(fd, &course, sizeof(struct Course)) > 0)) {
        strcpy(course_keys[n_course_keys], course.course_name);
        n_course_keys++;
    }
    close(fd);
    pthread_mutex_init(&mutex_course_keys, NULL);
}

int get_course_key(char course_name[NAME_SIZE]) {
    pthread_mutex_lock(&mutex_course_keys);
    for (int i = 0; i < n_course_keys; i++) {
        if (strcmp(course_keys[i], course_name) == 0) {
            pthread_mutex_unlock(&mutex_course_keys);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex_course_keys);
    return -1;
}

void get_lock_on_course_record(int fd, struct flock* lock, short lock_type, int rec_no) {
    lock->l_type = lock_type;
    lock->l_whence = SEEK_SET;
    lock->l_start = rec_no * sizeof(struct Course);
    lock->l_len = sizeof(struct Course);

    fcntl(fd, F_SETLKW, lock);
}

void get_lock_course_file(int fd, struct flock* lock, short lock_type) {
    lock->l_type = lock_type;
    lock->l_whence = SEEK_SET;
    lock->l_start = 0;
    lock->l_len = 0; // Lock the entire file

    fcntl(fd, F_SETLKW, lock);
}

void release_course_lock(int fd, struct flock* lock) {
    lock->l_type = F_UNLCK;
    fcntl(fd, F_SETLK, lock);
}

int add_course(struct Course course) {
  struct flock lock;
  int fd;
  fd = open(course_path, O_WRONLY);
  int rec_no = get_course_key(course.course_name);
  if(rec_no != -1) return error_duplicate_record; 
  course.active = 1;
  lseek(fd, 0, SEEK_END);
  write(fd, &course, sizeof(struct Course));
  pthread_mutex_lock(&mutex_course_keys);
  strcpy(course_keys[n_course_keys], course.course_name);
  n_course_keys++;
  pthread_mutex_unlock(&mutex_course_keys);
  close(fd);
  return 0;
}

struct Array_faculty_course get_faculty_courses(char faculty_name[NAME_SIZE]) {
  struct flock lock;
  int fd;
  struct Array_faculty_course matching_courses;
  int count = 0;

  fd = open(course_path, O_RDONLY);

  get_lock_course_file(fd, &lock, F_RDLCK);

  struct Course course;
  while (read(fd, &course, sizeof(struct Course)) > 0) {
    if (strcmp(course.facuilty_name, faculty_name) == 0) {
      matching_courses.arr[count++] = course;
    }
  }
  matching_courses.n = count;
  release_course_lock(fd, &lock);
  close(fd);

  return matching_courses;
}

int remove_course(struct Course course) {
  struct flock lock;
  int fd;

  fd = open(course_path, O_RDWR);
  int rec_no = get_course_key(course.course_name);
  if(rec_no == -1) return error_record_nonexistent;
  course.active = 0;
  get_lock_on_course_record(fd, &lock, F_WRLCK, rec_no);
  lseek(fd, rec_no* sizeof(struct Course), SEEK_SET);
  write(fd, &course, sizeof(struct Course));

  release_course_lock(fd, &lock);
  close(fd);
  return 0;
}
