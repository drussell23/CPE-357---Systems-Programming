/*
    Derek J. Russell 
    Dr. Christian Eckhardt
    Monitor 1 || Assignment #3  [CPE 357-11]
    November 3rd, 2021
*/
/*
    The parent process of Monitor #1:
    -------------------------------
    
    * The parent should check every 10 seconds, if the child was active.
      If not, kill (in Linux sense) the child and then end the parent 
      program. Prevent this process to be killed or terminated. Right 
      after the "kill" command, make sure to wait or you end up having 
      a zombie.
*/
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>

#define _


#define KBLU "x1B[34m" // Blue Color.
#define KNRM "\x1B[0m" // Normal Color. 

using namespace std;

// Function Prototypes. 
void HANDLE_SIGNAL_FUNCTION(int sign_num);
void doChildThings();


void HANDLE_SIGNAL_FUNCTION(int sign_num)
{
    cout << "Interception Signal... " << endl;
    return;
}

void doChildThings()
{
    string user_input;

    DIR *directory = opendir("..");
    struct dirent *directory_pointer;
    time_t timer = time(nullptr);
    struct tm tm = *localtime(&timer);
    
    struct stat st;
    struct stat sb;

    char cwd[2000];

     struct stat
    {
            dev_t st_dev;
            ino_t st_ino;
            mode_t st_mode;
            nlink_t st_nlink;
            uid_t st_uid;
            gid_t st_gid;
            dev_t st_rdev;
            off_t st_size;
        
       #if defined(__rtems__)
            struct timespec st_atim;
            struct timespec st_mim;
            struct timespec st_ctim;
            blksize_t st_blksize;
            blkcnt_t st_blocks;

        #else
        #if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
            time_t st_atime;
            time_t st_mtime;
            time_t st_ctime;
        #else 
           // time_t  st_atime at;
            long    st_sparel;
           // time_t  st_mtime mt;
            long    st_spare2;
           // time_t  st_ctime ct;
            long    st_spare3;
            long    st_blksize;
            long    st_blocks;
            long    st_spare4[2];
        #endif
        #endif  

        #define st_atime st_atim.tv_sec
        #define st_mtime st_mtim.tv_sec
        #define st_ctime st_ctim.tv_sec
    };

    bool again = true;

    while(again)
    {
        cout << "(1.) Enter 'list' lists the content of the current directory." << endl;
        cout << "(2.) Press any key to access the file status." << endl;
        cout << "(3.) Enter 'q' to QUIT the program.\n" << endl;
        
        cout << "stat prog$: ";
        cin >> user_input;
        
        cout << "\n========================================\n" << endl;

        if(user_input == "list")
        {
            // List all files in current directory. 
            //cout << "Time: " << tm.tm_hour << " (hr)" <<  " : " << tm.tm_min << " (min)" << endl;
            //getcwd(cwd, 2000);
            
            cout << "Here is your current working directory:" << endl;
            cout << "---------------------------------------" << endl;

            while((directory_pointer = readdir(directory)) != nullptr)
            {
                cout << directory_pointer->d_name << endl;
                cout << directory_pointer->d_type << endl;
                cout << directory_pointer->d_ino << endl;
                cout << directory_pointer->d_off << endl;
            }
            cout << "------------------------------------------" << endl;
            closedir(directory);
            sleep(100);
        }
        else if(user_input == "q")
        {
            // Quit the program. 
            cout << "Program Ending... Good Bye!" << endl;
            exit(0);
            return (void(EXIT_SUCCESS));
        }
        else
        {
            cout << "ID of containing device: " <<(uintmax_t) major(sb.st_dev) << (uintmax_t) minor(sb.st_dev) << "\n\n";
            
            cout << "File type:" << endl;
            cout << "---------------------------------------" << endl;
            // Print the file stats. 
            cout << "I-node number: " << (unsigned long)(sb.st_ino) << endl;
            cout << "Mode: " << (unsigned long)(sb.st_mode) << " (octal)" << endl;
            cout << "Link count: " << (long)(sb.st_nlink) << endl;
            cout << "Ownership: UID = " << (long)(sb.st_uid) << " GID = " << (long)(sb.st_gid) << endl;
            cout << "Preferred I/O block size: " << (long)(sb.st_blksize) << " bytes" << endl;
            cout << "File size: " << (unsigned long long)(sb.st_size) << " bytes" << endl;
            cout << "Blocks allocated: " << (unsigned long long)(sb.st_blocks) << "\n\n";    
        }
    }
    return; 
}

int main(const int argc, const char **argv)
{
    signal(SIGINT, HANDLE_SIGNAL_FUNCTION);
    signal(SIGKILL, HANDLE_SIGNAL_FUNCTION);
    signal(SIGTERM, HANDLE_SIGNAL_FUNCTION);
    signal(SIGQUIT, HANDLE_SIGNAL_FUNCTION);
    signal(SIGINT, HANDLE_SIGNAL_FUNCTION);

    string user_input;
    string current_Directory; 

    struct stat sb;
    int parentPID = getpid();

    pid_t w, childPID = fork();
    
    fflush(nullptr);

    //childPID = getpid(); // Gets the childPID.

    if(childPID == 0)
    {   
        assert(argc != 2);
        assert(lstat(argv[1], &sb) == -1);
        //assert(childPID != 0);
        
        cout << "The Child's PID: " << childPID << endl;
        
        doChildThings();

        if(argc == 1)
        {
            pause(); // Wait for signals. 
        }
        _exit(atoi(argv[1]));
        return 1;
    
    }
    else if(childPID == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else
    {
        int wstatus;

        // Parent. 
        while(true)
        // while(!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus))
        {
            // int wstatus;
            // WNOHANG 0x01
            // WUNTRACED 0x02
            // WNOHANG | WUNTRACED = 0x03 <-- bitwise OR
            w = waitpid(childPID, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);

            if(w == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            
         
            if(WIFSIGNALED(wstatus) == 1 || WIFEXITED(wstatus) == 1) // man waitpid
            {
                // Exit the parent program. 
                exit(0);
                return EXIT_SUCCESS;
            }
        
            if(WIFEXITED(wstatus) == true)
            {
                cout << "exited " << WEXITSTATUS(wstatus) << endl;
            }
            else if(WIFSIGNALED(wstatus) == true)
            {
                cout << "killed by signal " << WTERMSIG(wstatus) << endl;
            }
            else if(WIFSTOPPED(wstatus) == true)
            {
                cout << "stopped by signal " << WSTOPSIG(wstatus) << endl;
            }
            else if(WIFCONTINUED(wstatus) == true)
            {
                cout << "continued..." << endl;
            }
            sleep(1000);
        }
    }
    return 0;
}