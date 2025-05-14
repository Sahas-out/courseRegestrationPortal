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

void intialize_student_course_resources() {
    int fd = open(std_course_path, O_RDONLY);
    struct Student_course stu_course;
    while ((read(fd, &stu_course, sizeof(struct Student_course)) > 0)) {
        char key[NAME_SIZE * 2];
        snprintf(key, sizeof(key), "%s|%s", stu_course.student_name, stu_course.course_name);
        strcpy(student_course_keys[n_student_course_keys], key);
        n_student_course_keys++;
    }
    close(fd);
    pthread_mutex_init(&mutex_stu_course_keys, NULL);
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

void get_lock_on_student_course_record(int fd, struct flock* lock, short lock_type,int rec_no) {
    lock->l_type = lock_type;
    lock->l_whence = SEEK_SET;
    lock->l_start = (rec_no) * sizeof(struct Student_course);
    lock->l_len = sizeof(struct Student_course);

    fcntl(fd, F_SETLKW, lock);
}

void get_lock_student_course_file(int fd, struct flock* lock, short lock_type) {
    lock->l_type = lock_type;
    lock->l_whence = SEEK_SET;
    lock->l_start = 0;
    lock->l_len = 0; // Lock the entire file

    fcntl(fd, F_SETLKW, lock);
}

void release_student_course_lock(int fd, struct flock* lock) {
    lock->l_type = F_UNLCK;
    fcntl(fd, F_SETLK, lock);
}

int add_student_course(struct Student_course stu_course) {
    struct flock lock;
    int fd;
    fd = open(std_course_path, O_WRONLY);
    int rec_no = get_student_course_key(stu_course.student_name, stu_course.course_name);
    if(rec_no != -1) return error_duplicate_record;
    stu_course.denrolled = 0;
    lseek(fd, 0, SEEK_END);
    write(fd, &stu_course, sizeof(struct Student_course));
    pthread_mutex_lock(&mutex_stu_course_keys);
    char key[NAME_SIZE * 2];
    snprintf(key, sizeof(key), "%s|%s", stu_course.student_name, stu_course.course_name);
    strcpy(student_course_keys[n_student_course_keys], key);
    n_student_course_keys++;
    pthread_mutex_unlock(&mutex_stu_course_keys);
    close(fd);
    return 0;
}

struct Array_student_course get_student_course(char student_name[NAME_SIZE]) {
    struct flock lock;
    int fd;
    struct Array_student_course matching_courses;
    int count = 0;

    fd = open(std_course_path, O_RDONLY);
    get_lock_student_course_file(fd, &lock, F_RDLCK);

    struct Student_course stu_course;
    while (read(fd, &stu_course, sizeof(struct Student_course)) > 0) {
        if (strcmp(stu_course.student_name, student_name) == 0) {
            matching_courses.arr[count++] = stu_course;
        }
    }
    matching_courses.n = count;
    release_student_course_lock(fd, &lock);
    close(fd);

    return matching_courses;
}

int denroll_student_course(struct Student_course stu_course) {
    struct flock lock;
    int fd;

    fd = open(std_course_path, O_WRONLY);
    int rec_no = get_student_course_key(stu_course.student_name, stu_course.course_name);
    if(rec_no == -1) return error_record_nonexistent;
    get_lock_on_student_course_record(fd, &lock, F_WRLCK,rec_no);

    stu_course.denrolled = 1;
    lseek(fd,rec_no * sizeof(struct Student_course), SEEK_SET);
    write(fd, &stu_course, sizeof(struct Student_course));

    release_student_course_lock(fd, &lock);
    close(fd);
    return 0;
}
