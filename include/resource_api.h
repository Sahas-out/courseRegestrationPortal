#include "models.h"
#define ADMIN_USER "admin"
#define ADMIN_PASS "123"
#define resource_path "./resources/"
#define course_path resource_path "courses"
#define faculties_path resource_path "faculties"
#define std_course_path resource_path "student_courses"
#define student_path resource_path "students"
#define error_duplicate_record 101
#define error_record_nonexistent 102
#define success 0
void intialize_resources();

void intialize_student_resources();
int add_student(struct Student student);
struct Student get_student(char student_name[NAME_SIZE]);
int update_student(struct Student student);

void intialize_faculty_resources();
int add_faculty(struct Faculty faculty);
struct Faculty get_faculty(char faculty_name[NAME_SIZE]);
int update_faculty(struct Faculty faculty);

void intialize_student_course_resources();
int add_student_course(struct Student_course);
struct Array_student_course get_student_course(char student_name[NAME_SIZE]);
int denroll_student_course(struct Student_course);

void intialize_course_resources();
int add_course(struct Course);
struct Array_faculty_course get_faculty_courses(char faculty_name[NAME_SIZE]);
int remove_course(struct Course);


