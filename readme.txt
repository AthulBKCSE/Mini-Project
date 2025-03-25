README
# CMD TypeSwift - Typing Tutor

A terminal-based typing tutor designed to help programmers improve their typing speed and accuracy with programming concepts.

## Features

- **User System**: Admin and regular user roles with different privileges
- **Multiple Lessons**: Built-in programming concept lessons
- **Custom Lessons**: Via `myown.txt` file or admin-created lessons
- **Real-time Feedback**: Color-coded characters (green=correct, red=incorrect)
- **Detailed Statistics**: WPM, CPM, accuracy, and time tracking
- **Progress Tracking**: User statistics across sessions
- **Admin Features**: User management, lesson management

## Installation

### Linux

1. Install dependencies:
   ```bash
   sudo apt-get install build-essential libncurses-dev  # Debian/Ubuntu
   sudo dnf install gcc ncurses-devel  # Fedora
   sudo pacman -S gcc ncurses  # Arch Linux

Compile:
gcc main.c -o typeswift -lncurses

Run:
./typeswift

	MAC:
Install Xcode Command Line Tools:
xcode-select --install
Install ncurses (if not already installed):
brew install ncurses
Compile the program:
gcc main.c -o typeswift -lncurses
Run the program:
./typeswift


First Run
On first run, an admin user will be created automatically:

Username: admin

Password: admin123

Usage
Launch the program and login

Regular users can:

Select and complete typing lessons

View their statistics

Use custom lessons from myown.txt

Admin users can additionally:

Create/delete users

Add/remove lessons

View all user statistics

Custom Lessons
For Regular Users:
Create a file named myown.txt in the same directory as the program

Add your custom text to the file

Select "Custom Lesson" from the menu

For Admin Users:
Select "Add Custom Lesson" from the admin menu

Enter a title and content for the lesson

The lesson will be saved and available to all users

Commands
Use number keys (1-9) to navigate menus

Press ESC during a lesson to cancel

Press any key to continue when prompted

Uninstallation
Simply delete the compiled executable file (typeswift). User data is stored in the users directory and lessons in the lessons directory - delete these if you want to remove all data.

License
This is free software. Use it for educational purposes.


## Key Enhancements Based on SDS

1. **User System**: Implemented Admin and Regular User roles as specified
2. **Detailed Statistics**: Added WPM, CPM, accuracy calculations with session tracking
3. **Platform Support**: Specific instructions for both Linux and macOS
4. **Lesson Management**: Admins can add new lessons for all users
5. **User Management**: Admins can create/delete users and view all stats
6. **Progress Tracking**: Users can see their improvement over time
7. **File Structure**: Organized user data and lessons in separate directories

The implementation follows the SDS document closely while adding the requested features for user roles and detailed statistics. The code is modular as specified in the SDS and includes all the required functionality.

Would you like me to explain any specific part of the implementation in more detail or make any adjustments to the features?
