#include "models.h"
#include "server.h"


void get_user_credentials(struct Authentication_token* auth_tok);
void display_help_menu();
int get_user_request();

struct Student add_student();
void display_student(struct Student student);
void update_student(struct Student* student);
void get_student_name(char buf[NAME_SIZE]);

struct Faculty add_faculty();
void display_faculty(struct Faculty faculty);
void update_faculty(struct Faculty* faculty);
void get_faculty_name(char buf[NAME_SIZE]);

struct Student_course get_student_course();
void display_student_course(struct Array_student_course,char stu_name[NAME_SIZE]); 

struct Course get_course();
void display_fac_courses(struct Array_faculty_course,char fac_name[NAME_SIZE]);

