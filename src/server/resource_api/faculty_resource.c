#include "resource_api.h"
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define MAX_RECORDS 1000

char faculty_keys[MAX_RECORDS][NAME_SIZE];
int n_faculty_keys = 0;
pthread_mutex_t mutex_fac_keys;

void intialize_faculty_resources() {
    int fd = open(faculties_path, O_RDONLY);
    struct Faculty fac;
    while ((read(fd, &fac, sizeof(struct Faculty)) > 0)) {
        strcpy(faculty_keys[n_faculty_keys], fac.faculty_name);
        n_faculty_keys++;
    }
    close(fd);
    pthread_mutex_init(&mutex_fac_keys, NULL);
}

int get_faculty_key(char match_value[NAME_SIZE]) {
    pthread_mutex_lock(&mutex_fac_keys);
    for (int i = 0; i < n_faculty_keys; i++) {
        if (strcmp(faculty_keys[i], match_value) == 0) {
            pthread_mutex_unlock(&mutex_fac_keys);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex_fac_keys);
    return -1;
}

void get_lock_on_faculty_record(int fd, struct flock* lock, short lock_type, char match_value[NAME_SIZE]) {
    lock->l_type = lock_type;
    lock->l_whence = SEEK_SET;
    lock->l_start = (get_faculty_key(match_value)) * sizeof(struct Faculty);
    lock->l_len = sizeof(struct Faculty);

    fcntl(fd, F_SETLKW, lock);
}

void release_lock_faculty(int fd, struct flock* lock) {
    lock->l_type = F_UNLCK;
    fcntl(fd, F_SETLK, lock);
}

void add_faculty(struct Faculty faculty) {
    struct flock lock;
    int fd;
    fd = open(faculties_path, O_WRONLY);
    faculty.active = 1;
    lseek(fd, 0, SEEK_END);
    write(fd, &faculty, sizeof(struct Faculty));
    pthread_mutex_lock(&mutex_fac_keys);
    strcpy(faculty_keys[n_faculty_keys], faculty.faculty_name);
    n_faculty_keys++;
    pthread_mutex_unlock(&mutex_fac_keys);
    close(fd);
}

void update_faculty(struct Faculty faculty) {
    struct flock lock;
    int fd;
    fd = open(faculties_path, O_WRONLY);
    get_lock_on_faculty_record(fd, &lock, F_WRLCK, faculty.faculty_name);
    lseek(fd, get_faculty_key(faculty.faculty_name) * sizeof(struct Faculty), SEEK_SET);
    write(fd, &faculty, sizeof(struct Faculty));
    release_lock_faculty(fd, &lock);
    close(fd);
}

struct Faculty get_faculty(char faculty_name[NAME_SIZE]) {
    struct flock lock;
    int fd;
    fd = open(faculties_path, O_RDONLY);
    get_lock_on_faculty_record(fd, &lock, F_RDLCK, faculty_name);
    lseek(fd, get_faculty_key(faculty_name) * sizeof(struct Faculty), SEEK_SET);
    struct Faculty faculty;
    read(fd, &faculty, sizeof(struct Faculty));
    release_lock_faculty(fd, &lock);
    close(fd);
    return faculty;
}
