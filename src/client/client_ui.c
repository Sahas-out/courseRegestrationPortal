#include "client_ui.h"
#include <stdio.h>
#include <string.h>

void get_user_credentials(struct Authentication_token* auth_tok){
  printf("Enter username(type admin as username for ADMIN): ");
  fgets(auth_tok->user_name, NAME_SIZE, stdin);
  auth_tok->user_name[strcspn(auth_tok->user_name, "\n")] = '\0';
  printf("Enter password: ");
  fgets(auth_tok->pass, NAME_SIZE, stdin);
  auth_tok->pass[strcspn(auth_tok->pass, "\n")] = '\0';
}
// Function to add a new student
struct Student add_student()
{
  struct Student new_student;

  printf("Enter student name: ");
  fgets(new_student.student_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_student.student_name[strcspn(new_student.student_name, "\n")] = '\0';

  printf("Enter student password: ");
  fgets(new_student.user_password, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_student.user_password[strcspn(new_student.user_password, "\n")] = '\0';

  printf("Enter student grade: ");
  scanf("%f", &new_student.student_grade);
  getchar(); // Consume the leftover newline character

  new_student.active = 1;

  return new_student;
}

// Function to display a student's information
void display_student(struct Student student)
{
  printf("\n--- Student Information ---\n");
  printf("Name: %s\n", student.student_name);
  printf("Grade: %.2f\n", student.student_grade);
  printf("Password: %s\n", student.user_password);
  printf("Active: %d\n",student.active); 
  printf("---------------------------\n");
}

// Function to update a student's grade
void update_student(struct Student *student)
{

  int choice;
  do {
    display_student(*student);
    printf("\n--- Update Student Menu ---\n");
    printf("1. Update Grade\n");
    printf("2. Update Password\n");
    printf("3. Update Active State\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the leftover newline character

    switch (choice) {
      case 1:
        printf("Enter new grade for the student: ");
        scanf("%f", &student->student_grade);
        getchar(); // Consume the leftover newline character
        break;
      case 2:
        printf("Enter new password for the student: ");
        fgets(student->user_password, NAME_SIZE, stdin);
        student->user_password[strcspn(student->user_password, "\n")] = '\0';
        break;
      case 3:
        printf("Enter new active state (1 for active, 0 for inactive): ");
        scanf("%d", &student->active);
        getchar(); // Consume the leftover newline character
        break;
      case 4:
        printf("Exiting update menu.\n");
        break;
      default:
        printf("Invalid choice. Please try again.\n");
    }
  } while (choice != 4);
}

// Function to get a student's name
void get_student_name(char buf[NAME_SIZE])
{
  printf("Enter the student's name: ");
  fgets(buf, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  buf[strcspn(buf, "\n")] = '\0';
}

// Function to add a new faculty
struct Faculty add_faculty()
{
  struct Faculty new_faculty;

  printf("Enter faculty name: ");
  fgets(new_faculty.faculty_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_faculty.faculty_name[strcspn(new_faculty.faculty_name, "\n")] = '\0';

  printf("Enter faculty password: ");
  fgets(new_faculty.user_password, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_faculty.user_password[strcspn(new_faculty.user_password, "\n")] = '\0';

  printf("Enter faculty salary: ");
  scanf("%d", &new_faculty.faculty_salary);
  getchar(); // Consume the leftover newline character

  new_faculty.active = 1;

  return new_faculty;
}

// Function to display a faculty's information
void display_faculty(struct Faculty faculty)
{
  printf("\n--- Faculty Information ---\n");
  printf("Name: %s\n", faculty.faculty_name);
  printf("Salary: %d\n", faculty.faculty_salary);
  printf("Pass: %s\n", faculty.user_password);
  printf("Active: %d\n", faculty.active);
  printf("---------------------------\n");
}

// Function to update a faculty's salary
void update_faculty(struct Faculty *faculty)
{
  int choice;
  do {
    display_faculty(*faculty);
    printf("\n--- Update Faculty Menu ---\n");
    printf("1. Update Salary\n");
    printf("2. Update Password\n");
    printf("3. Update Active State\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the leftover newline character

    switch (choice) {
      case 1:
        printf("Enter new salary for the faculty: ");
        scanf("%d", &faculty->faculty_salary);
        getchar(); // Consume the leftover newline character
        break;
      case 2:
        printf("Enter new password for the faculty: ");
        fgets(faculty->user_password, NAME_SIZE, stdin);
        faculty->user_password[strcspn(faculty->user_password, "\n")] = '\0';
        break;
      case 3:
        printf("Enter new active state (1 for active, 0 for inactive): ");
        scanf("%d", &faculty->active);
        getchar(); // Consume the leftover newline character
        break;
      case 4:
        printf("Exiting update menu.\n");
        break;
      default:
        printf("Invalid choice. Please try again.\n");
    }
  } while (choice != 4);
}
// Functioo get a faculty's name
void get_faculty_name(char buf[NAME_SIZE])
{
  printf("Enter the faculty's name: ");
  fgets(buf, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  buf[strcspn(buf, "\n")] = '\0';
}

// Function to get input for a Student_course structure
struct Student_course get_student_course()
{
  struct Student_course new_student_course;

  printf("Enter student name: ");
  fgets(new_student_course.student_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_student_course.student_name[strcspn(new_student_course.student_name, "\n")] = '\0';

  printf("Enter course name: ");
  fgets(new_student_course.course_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_student_course.course_name[strcspn(new_student_course.course_name, "\n")] = '\0';

  new_student_course.denrolled = 0; // Default value for denrolled is 0 (not denrolled)

  return new_student_course;
}
void display_student_course(struct Array_student_course courses,char stu_name[NAME_SIZE]){
  printf("\n--- Courses for Student: %s ---\n", stu_name);
  for (int i = 0; i < courses.n; i++) {
    printf("Course Name: %s, Denrolled: %d\n", courses.arr[i].course_name, courses.arr[i].denrolled);
  }
  printf("---------------------------\n");
}

// Function to get input for a Course structure
struct Course get_course()
{
  struct Course new_course;

  printf("Enter course name: ");
  fgets(new_course.course_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_course.course_name[strcspn(new_course.course_name, "\n")] = '\0';

  printf("Enter faculty name for the course: ");
  fgets(new_course.facuilty_name, NAME_SIZE, stdin);
  // Remove trailing newline character if present
  new_course.facuilty_name[strcspn(new_course.facuilty_name, "\n")] = '\0';

  new_course.active = 1; // Default value for active is 1 (active)

  return new_course;
}

// Function to display courses taught by a faculty
void display_fac_courses(struct Array_faculty_course courses, char fac_name[NAME_SIZE])
{
  printf("\n--- Courses for Faculty: %s ---\n", fac_name);
  for (int i = 0; i < courses.n; i++) {
    printf("Course Name: %s, Active: %d\n", courses.arr[i].course_name, courses.arr[i].active);
  }
  printf("---------------------------\n");
}
// Function to display the help menu

void display_admin_help_menu(){
  printf("\n--- Help Menu ---\n");
  printf("Enter the corresponding number for the desired action:\n");
  printf("%d. Add a new student (ADD_STUDENT)\n", ADD_STUDENT);
  printf("%d. Update a student's details (UPDATE_STUDENT)\n", UPDATE_STUDENT);
  printf("%d. Display a student's details (DISPLAY_STUDENT)\n", DISPLAY_STUDENT);
  printf("%d. Add a new faculty (ADD_FACULTY)\n", ADD_FACULTY);
  printf("%d. Update a faculty's details (UPDATE_FACULTY)\n", UPDATE_FACULTY);
  printf("%d. Display a faculty's details (DISPLAY_FACULTY)\n", DISPLAY_FACULTY);
  printf("%d. Add a student-course association (ADD_STUDENT_COURSE)\n", ADD_STUDENT_COURSE);
  printf("%d. Get all courses for a student (GET_STUDENT_COURSE)\n", GET_STUDENT_COURSE);
  printf("%d. Denroll a student from a course (DENROLL_STUDENT_COURSE)\n", DENROLL_STUDENT_COURSE);
  printf("%d. Add a new course (ADD_COURSE)\n", ADD_COURSE);
  printf("%d. Get all courses taught by a faculty (GET_FACULTY_COURSE)\n", GET_FACULTY_COURSE);
  printf("%d. Remove a course (REMOVE_COURSE)\n", REMOVE_COURSE);
  printf("%d. Logout (LOGOUT)\n", LOGOUT);
  printf("---------------------------\n");
}
void display_student_help_menu(){
  printf("\n--- Help Menu ---\n");
  printf("Enter the corresponding number for the desired action:\n");
  printf("%d. Update a student's details (UPDATE_STUDENT)\n", UPDATE_STUDENT);
  printf("%d. Display a student's details (DISPLAY_STUDENT)\n", DISPLAY_STUDENT);
  printf("%d. Add a student-course association (ADD_STUDENT_COURSE)\n", ADD_STUDENT_COURSE);
  printf("%d. Get all courses for a student (GET_STUDENT_COURSE)\n", GET_STUDENT_COURSE);
  printf("%d. Denroll a student from a course (DENROLL_STUDENT_COURSE)\n", DENROLL_STUDENT_COURSE);
  printf("%d. Logout (LOGOUT)\n", LOGOUT);
  printf("---------------------------\n");
}
void display_faculty_help_menu(){
  printf("\n--- Help Menu ---\n");
  printf("Enter the corresponding number for the desired action:\n");
  printf("%d. Update a faculty's details (UPDATE_FACULTY)\n", UPDATE_FACULTY);
  printf("%d. Display a faculty's details (DISPLAY_FACULTY)\n", DISPLAY_FACULTY);
  printf("%d. Add a new course (ADD_COURSE)\n", ADD_COURSE);
  printf("%d. Get all courses taught by a faculty (GET_FACULTY_COURSE)\n", GET_FACULTY_COURSE);
  printf("%d. Remove a course (REMOVE_COURSE)\n", REMOVE_COURSE);
  printf("%d. Logout (LOGOUT)\n", LOGOUT);
  printf("---------------------------\n");
}
void display_help_menu(enum Role role) {
  if(role == STUDENT) display_student_help_menu();
  else if(role == FACULTY) display_faculty_help_menu();
  else display_admin_help_menu();
}
int get_user_request(){
  printf("Enter your choice\n");
  int usr_req;
  scanf("%d",&usr_req);
  getchar();
  return usr_req;
}
