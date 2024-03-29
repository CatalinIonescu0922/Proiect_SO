#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

int main (int argc , char **argv){
    FILE *fp;
    DIR *dir;
    struct dirent *dp;
    char filepath[50];
    struct stat file_info;
    if(argc!=2){
        printf("eroare la deschidere");
        exit(-1);
    }
    if(dir = opendir(argv[1])==NULL){
        perror("ERORR ON FOLDER");
        exit(-1);
    }  
    if(fp=fopen("out.txt","w")==NULL){
         printf("eroare la fis de out");
         exit(-1);
    }
    while (dp=readdir(dir)!=NULL)
    {
        if(strcmp(dp->d_name,".")== 0 && strcmp(dp->d_name,"..")){
        continue;   
        } else{
        sprintf(filepath, "%s/%s", argv[1], dp->d_name);             
        }
        if(stat(filepath,&file_info)<0){
            printf("err stat");
            exit(-1);
        }
        if(S_ISDIR(file_info.st_mode)==1){
            // PARCURGERE FISIER FUCNTIE FACUTA AICI 
            
        }

    }
    

    
}