#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define MAX_LESSONS 30
#define MAX_TEXT_LENGTH 2000
#define MAX_LINE_LENGTH 256
#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define MAX_USERS 10
#define USER_DIR "users"
#define LESSON_DIR "lessons"

typedef struct {
    char username[50];
    char password[50];
    int isAdmin;
    int stats_correct;
    int stats_incorrect;
    double stats_time;
    int stats_sessions;
} User;

typedef struct {
    char title[50];
    char filename[50];
    char content[MAX_TEXT_LENGTH];
} Lesson;

User users[MAX_USERS];
int userCount = 0;
User currentUser;
Lesson lessons[MAX_LESSONS];
int lessonCount = 0;

/* ==================== CENTERING HELPERS ==================== */
int get_center_pos(int text_length) {
    int max_x = getmaxx(stdscr);
    return (max_x - text_length) / 2;
}

void print_centered(int y, const char* text) {
    mvprintw(y, get_center_pos(strlen(text)), "%s", text);
}

void print_centered_multiline(int start_y, const char* text) {
    char buffer[MAX_TEXT_LENGTH];
    strcpy(buffer, text);
    
    char* line = strtok(buffer, "\n");
    int y = start_y;
    
    while (line != NULL) {
        print_centered(y++, line);
        line = strtok(NULL, "\n");
    }
}

void draw_centered_box(int y, int width, const char* title) {
    int max_x = getmaxx(stdscr);
    int start_x = (max_x - width) / 2;
    
    // Top border
    mvaddch(y, start_x, ACS_ULCORNER);
    for (int i = 1; i < width-1; i++) mvaddch(y, start_x+i, ACS_HLINE);
    mvaddch(y, start_x+width-1, ACS_URCORNER);
    
    // Title
    if (title) {
        int title_x = start_x + (width - strlen(title) - 2) / 2;
        mvprintw(y, title_x, " %s ", title);
    }
    
    // Bottom border
    mvaddch(y+2, start_x, ACS_LLCORNER);
    for (int i = 1; i < width-1; i++) mvaddch(y+2, start_x+i, ACS_HLINE);
    mvaddch(y+2, start_x+width-1, ACS_LRCORNER);
}

/* ==================== CORE FUNCTIONS ==================== */
void initDirectories() {
    mkdir(USER_DIR, 0700);
    mkdir(LESSON_DIR, 0700);
}

void loadUsers() {
    FILE *file = fopen(USER_DIR "/users.dat", "rb");
    if (file != NULL) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    } else {
        userCount = 0;
    }
}

void saveUsers() {
    FILE *file = fopen(USER_DIR "/users.dat", "wb");
    if (file != NULL) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

void loadLessons() {
    /* ... (keep your existing loadLessons() implementation exactly the same) ... */
}

int getNumericInput(int min, int max) {
    char input[10];
    int value = 0;
    int y, x;
    getyx(stdscr, y, x);
    
    echo();
    curs_set(1);
    mvgetstr(y, x, input);
    curs_set(0);
    noecho();
    
    value = atoi(input);
    
    while (value < min || value > max) {
        char prompt[50];
        snprintf(prompt, sizeof(prompt), "Invalid input! Enter a number (%d-%d): ", min, max);
        print_centered(y, prompt);
        refresh();
        
        echo();
        curs_set(1);
        mvgetstr(y, get_center_pos(0) + strlen(prompt), input);
        curs_set(0);
        noecho();
        
        value = atoi(input);
    }
    
    return value;
}

int confirmAction(const char* message) {
    int y, x;
    getyx(stdscr, y, x);
    
    char full_message[100];
    snprintf(full_message, sizeof(full_message), "%s (y/n): ", message);
    print_centered(y, full_message);
    refresh();
    
    int ch = getch();
    return (ch == 'y' || ch == 'Y');
}

/* ==================== MENU FUNCTIONS ==================== */
int login() {
    clear();
    int y = 2;
    
    print_centered(y++, "CMD TypeSwift - Typing Tutor");
    y++;
    
    draw_centered_box(y, 40, "Main Menu");
    y += 1;
    
    print_centered(y++, "1. Login");
    print_centered(y++, "2. Create New User");
    print_centered(y++, "3. Exit");
    y++;
    
    print_centered(y, "Select an option: ");
    refresh();
    
    int choice = getNumericInput(1, 3);
    
    if (choice == 3) {
        return 0;
    } else if (choice == 2) {
        createUser();
        return login();
    } else if (choice == 1) {
        char username[50];
        char password[50];
        
        clear();
        print_centered(2, "Username: ");
        echo();
        move(2, get_center_pos(0) + 10);
        getstr(username);
        
        print_centered(3, "Password: ");
        noecho();
        move(3, get_center_pos(0) + 10);
        getstr(password);
        echo();
        
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0 && 
                strcmp(users[i].password, password) == 0) {
                currentUser = users[i];
                return 1;
            }
        }
        
        print_centered(5, "Invalid username or password!");
        print_centered(6, "Press any key to continue...");
        refresh();
        getch();
        return login();
    } else {
        return login();
    }
}

void displayMainMenu() {
    clear();
    int y = 1;
    
    print_centered(y++, "CMD TypeSwift - Main Menu");
    y += 2;
    
    print_centered(y++, "1. Lessons Menu");
    print_centered(y++, "2. Custom Lesson (from myown.txt)");
    if (currentUser.isAdmin) {
        print_centered(y++, "3. Add Custom Lesson");
    }
    print_centered(y++, "%d. Help", currentUser.isAdmin ? 4 : 3);
    print_centered(y++, "%d. Logout", currentUser.isAdmin ? 5 : 4);
    y++;
    
    print_centered(y, "Select an option (1-%d): ", currentUser.isAdmin ? 5 : 4);
    refresh();
}

void lessonsMenu() {
    int choice;
    do {
        clear();
        int y = 1;
        
        print_centered(y++, "CMD TypeSwift - Lessons Menu");
        y += 2;
        
        print_centered(y++, "Available Lessons:");
        y++;
        
        for (int i = 0; i < lessonCount; i++) {
            const char* title = lessons[i].title;
            if (title[0] >= '0' && title[0] <= '9' && title[1] == '.') {
                title += 3;
            }
            char entry[60];
            snprintf(entry, sizeof(entry), "%d. %s", i+1, title);
            print_centered(y++, entry);
        }
        
        char back_option[30];
        snprintf(back_option, sizeof(back_option), "%d. Back to Main Menu", lessonCount+1);
        print_centered(y++, "");
        print_centered(y++, back_option);
        y++;
        
        print_centered(y, "Select a lesson (1-%d): ", lessonCount+1);
        refresh();
        
        choice = getNumericInput(1, lessonCount+1);
        
        if (choice >= 1 && choice <= lessonCount) {
            runLesson(choice - 1);
        }
    } while (choice != lessonCount + 1);
}

/* ==================== LESSON FUNCTIONS ==================== */
void runLesson(int lessonIndex) {
    /* ... (keep your existing runLesson implementation) ... */
}

void runCustomLesson() {
    /* ... (keep your existing runCustomLesson implementation) ... */
}

/* ==================== ADMIN FUNCTIONS ==================== */
void adminMenu() {
    int choice;
    do {
        clear();
        int y = 1;
        
        print_centered(y++, "CMD TypeSwift - Admin Menu");
        y += 2;
        
        print_centered(y++, "1. Manage Users");
        print_centered(y++, "2. Manage Lessons");
        print_centered(y++, "3. View All Users");
        print_centered(y++, "4. User Mode");
        print_centered(y++, "5. Logout");
        y++;
        
        print_centered(y, "Select an option (1-5): ");
        refresh();
        
        choice = getNumericInput(1, 5);
        
        switch (choice) {
            case 1: {
                clear();
                y = 1;
                print_centered(y++, "User Management");
                y++;
                
                print_centered(y++, "1. Create User");
                print_centered(y++, "2. Delete User");
                print_centered(y++, "3. Back");
                y++;
                
                print_centered(y, "Select an option: ");
                refresh();
                
                int userChoice = getNumericInput(1, 3);
                if (userChoice == 1) {
                    createUser();
                } else if (userChoice == 2) {
                    deleteUser();
                }
                break;
            }
            case 2:
                manageLessons();
                break;
            case 3:
                viewAllUsers();
                break;
            case 4:
                userMenu();
                break;
        }
    } while (choice != 5);
}

/* ==================== USER MANAGEMENT ==================== */
void createUser() {
    /* ... (update to use centered functions similar to login) ... */
}

void deleteUser() {
    /* ... (update to use centered functions) ... */
}

void viewAllUsers() {
    /* ... (update to use centered functions) ... */
}

/* ==================== LESSON MANAGEMENT ==================== */
void manageLessons() {
    /* ... (update to use centered functions) ... */
}

void addCustomLesson() {
    /* ... (update to use centered functions) ... */
}

/* ==================== MAIN FUNCTIONS ==================== */
void userMenu() {
    /* ... (keep your existing implementation) ... */
}

void displayHelp() {
    clear();
    int y = 1;
    
    print_centered(y++, "CMD TypeSwift - Help");
    y += 2;
    
    print_centered_multiline(y, 
        "This typing tutor helps you improve your typing speed and accuracy.\n\n"
        "Features:\n"
        "- Multiple programming concept lessons\n"
        "- Custom lessons from myown.txt file\n"
        "- Admin/user system with different privileges\n"
        "- Real-time visual feedback (green=correct, red=incorrect)\n"
        "- Detailed typing statistics (WPM, CPM, accuracy, time taken)\n"
        "- User progress tracking across sessions\n\n"
        "Instructions:\n"
        "1. Select a lesson from the main menu\n"
        "2. Type the displayed text as accurately as possible\n"
        "3. View your statistics at the end\n"
        "4. Press ESC during a lesson to cancel\n\n"
        "Admin Features:\n"
        "- Create/delete users\n"
        "- Add/remove lessons\n"
        "- View all user statistics");
    
    y += 15; // Adjust based on text length
    print_centered(y, "Press any key to return to menu...");
    refresh();
    getch();
}

void displayStats(int correct, int incorrect, double timeTaken) {
    /* ... (update to use centered functions) ... */
}

void printColoredText(const char *original, const char *typed) {
    /* ... (keep your existing implementation) ... */
}

int main() {
    /* ... (keep your existing main() implementation) ... */
}
