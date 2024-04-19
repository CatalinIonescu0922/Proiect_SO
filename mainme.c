#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

void path_stat(const char *path)
{
    FILE *f;
    struct stat st;
    stat(path, &st);
    if((f=fopen("out.txt","w+"))==NULL){
            printf("eroare la deshidere");
            exit(-1);
        }
        if (S_ISDIR(st.st_mode))
    {
        printf("DIR - %s\n", path);
    }
    else
    {
        printf("FILE - %s\n", path);
    }
    printf("Time of Last Status Change - %ld\n", st.st_ctime);

    if (S_ISDIR(st.st_mode))
    {
        fprintf(f,"%s %s\n","DIR - ", path);
        fprintf(f,"%ld",st.st_ctime);
    }
    else
    {
        fprintf(f,"%s %s\n","FILE - ", path);
        fprintf(f,"%ld",st.st_ctime);
    }

    printf("Size - %ld\n", st.st_size);
    printf("Time of Last Modification - %ld\n", st.st_mtime);
    printf("Time of Last Status Change - %ld\n", st.st_ctime);
    
    printf("\n");
}

void parcurgere(const char *path)
{
    struct stat st;
    stat(path, &st);

    if (S_ISDIR(st.st_mode))
    {
        struct dirent *db;
        DIR *director = opendir(path);

        while ((db = readdir(director)) != NULL)
        {
            char newPath[300];
            sprintf(newPath, "%s/%s", path, db->d_name);

            if (strcmp(db->d_name, ".") && strcmp(db->d_name, ".."))
            {
                parcurgere(newPath);
            }
        }

        closedir(director);
    }

    path_stat(path);
}

int main(int argc, char *argv[])

{
    int status ;
    pid_t pid[argc];
    // for(int i=1;i<argc;i++){
    //     pid[i]=fork();
    //     if(pid[i]==0){
    //         // child process
    //         parcurgere(argv[i]);
    //         exit(i);
    //     } 
    // }

    // for(int i=1;i<argc;i++){
    //     wait(&status);
    // }
// for(i=0; i<argc-3; i++)
//     {
//         if((pid[i]=fork())<0)
//         {
//             perror("Error at fork\n");
//             exit(2);
//         }

//         if(pid[i]==0)
//         {
//             //write the snapshot
//             exit(0);
//         }
//     }

//     for(i=1; i<=argc-3; i++)
//     {
//         wpid=wait(&status);
//         if(WIFEXISTED(status))  printf("Child %d ended with code %d\n", wpid, WIFEXISTED(status));
//         else                    printf("Child %d ended abnormally\n", wpid);
//     }

//     fclose(file);

//     return 0;
// } 

    
   return 0 ;
}