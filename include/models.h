#define NAME_SIZE 30
struct Course
{
  char course_name[NAME_SIZE];
  char facuilty_name[NAME_SIZE];
  int active;
};

struct Faculty
{
  char faculty_name[NAME_SIZE];
  int faculty_salary;
  char user_password[NAME_SIZE];  
  int active;
};

struct Student_course
{
  char student_name[NAME_SIZE];
  char course_name[NAME_SIZE];
  int denrolled;
};

struct Student
{
  char student_name[NAME_SIZE];
  float student_grade;
  char user_password[NAME_SIZE];
  int active;
};

struct Array_student_course
{
  struct Student_course arr[10];
  int n;
};

struct Array_faculty_course
{
  struct Course arr[10];
  int n;
};


