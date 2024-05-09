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

typedef struct Date_fisiere
{
    ino_t unic_id;
    time_t ultimul_timp;
    int size_nume_fisier;
    char nume_fisier[100];
} Date_fisiere;

void moveFile(const char* source_path, const char* destination_path) {
    char *last_slash = strrchr(source_path, '/');
    char *filename = last_slash + 1;
    char new_path[102];
    strcpy(new_path,destination_path);
    strcat(new_path,"/");
    strcat(new_path,filename);
    printf("%s \n",new_path);
    if (rename(source_path,new_path) == 0) {
        printf("File moved successfully.\n"); 
    } else {
        perror("Error moving file");
        exit(-1);
    }
}

void mutare_daca_gasesc_virusi(const char *script_path,const char *file_path,const char* destination_path){
      int fd[2]; // fd[0] citire fd[1] scriere
      pid_t pid;
      int status;
      char returnare_script[3];
      if(pipe(fd)<0){
         printf("eroare la creare pipe");
         exit(-1);
      }
      if((pid=fork())<0){
          printf("eroare la fork");
          exit(-1);
      }
      if(pid==0){
        // child proces 
        close(fd[0]); //vreau sa inchid citirea deoarece in procesul copil eu vreau sa scriu in pipe
        dup2(fd[1],1);
          if(execlp("./script.sh","./script.sh",file_path,NULL) == -1)
            {
                perror("\nOops, eroare la script\n");
                exit(-12);
            } 
        // exit(-1);
      } else{
          pid_t wpid;
          close(fd[1]); // inchidem capul de scriere;
          wpid=wait(&status);
           if (WIFEXITED(status)) 
            {
                printf("\nPid-ul procesului pentru ultima modificare %d a ieșit cu codul: %d\n", wpid, WEXITSTATUS(status));
            } 
            else 
            {
                fprintf(stderr, "Eroare la așteptarea procesului\n");
            }
            if(read(fd[0],returnare_script,3*sizeof(char))!=3){
                printf("nu am citit ce trbuie");
                exit(-1);
            }
            close(fd[0]); // deja am citit ce am vrut 
            if(strcmp(returnare_script,"rau")){
                moveFile(file_path,destination_path);
            } 
      }

}
void update_time_fisier(Date_fisiere *vector,int posizitie,int fd,time_t timp_nou){
   int offset_modificare=0;
   off_t offset;
   // fac offset ul de seek_set plus cat calculez eu offset ul 
   offset_modificare+=(posizitie+1)*8;
   offset_modificare+= posizitie*8;
   offset_modificare+=posizitie*4;
   for(int i=0;i<posizitie;i++){
       offset_modificare+=vector[i].size_nume_fisier;
   }
   offset=lseek(fd,offset_modificare,SEEK_SET);
   if(write(fd,&(timp_nou),sizeof(time_t))!=sizeof(time_t)){
       perror("Error writing modification time");
       exit(EXIT_FAILURE);
   }
}

void adaugare_fisier( ino_t identificator, time_t timp,int fd,int size_of_path, const char *path){
    off_t offset;
    offset = lseek(fd, 0, SEEK_END);
    if (offset == -1) {
        perror("Error seeking to end of file");
        exit(EXIT_FAILURE);
    }
    if (write(fd, &(identificator), sizeof(ino_t)) != sizeof(ino_t)) 
        {
        perror("Error writing inode number");
        exit(EXIT_FAILURE);
        }

// Write the modification time (st_mtime) to the file
    if (write(fd, &(timp), sizeof(time_t)) != sizeof(time_t)) 
        {
            perror("Error writing modification time");
            exit(EXIT_FAILURE);
        }
    if (write(fd, &(size_of_path), sizeof(int)) != sizeof(int)) 
        {
            perror("Error writing modification time");
            exit(EXIT_FAILURE);
        }
    if (write(fd, path, size_of_path*sizeof(char)) != size_of_path*sizeof(char)) 
        {
            perror("Error writing modification time");
            exit(EXIT_FAILURE);
        }
}
Date_fisiere *returnare_fisiere(Date_fisiere *vector)
{
  int i=0;
  int fd;
  ino_t inode_number;
  time_t modification_time;
  int size_of_path;
  char path[100];
  int citire_ino=1;

  fd=open("outn.txt",O_RDWR | O_CREAT);
    if (fd < 0) {
        perror("eroare la deschidere");
        exit(1);
    }

  while (citire_ino!=0)
  {
    if ((citire_ino=read(fd, &inode_number, sizeof(ino_t))) != sizeof(ino_t)) 
    {
            break;
    }

// Read the modification time (st_mtime) from the file
    if (read(fd, &modification_time, sizeof(time_t)) != sizeof(time_t)) {
        perror("Error reading modification time");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &size_of_path, sizeof(int)) != sizeof(int)) {
        perror("Error reading size");
        exit(EXIT_FAILURE);
    }
    if (read(fd, path, size_of_path * sizeof(char)) != size_of_path * sizeof(char)) {
        // printf("eroare la citire path\n");
        // break;
        perror("Error reading path");
        exit(EXIT_FAILURE);
    }
    path[size_of_path] = '\0';
    vector[i].unic_id=inode_number;
    vector[i].ultimul_timp=modification_time;
    vector[i].size_nume_fisier=size_of_path;
    strcpy(vector[i].nume_fisier,path);
    ++i;
  }
  close(fd);
  return vector;
}

void verificare_modificari(const char *path,int fd)
{
    struct stat st;
    struct stat st_verificare_fisier;
    Date_fisiere *toate_fisiere = malloc(100 * sizeof(Date_fisiere));
    stat(path, &st);
     if (fstat(fd, &st_verificare_fisier) == -1) {
        perror("Error getting file status");
        exit(EXIT_FAILURE);
    }

    // Check if the file is empty
    if (st_verificare_fisier.st_size == 0) {
        adaugare_fisier(st.st_ino,st.st_mtime,fd,strlen(path),path);
        printf("am adaugat fisierul %s\n",path);
    } else {
        toate_fisiere=returnare_fisiere(toate_fisiere);
        int apare_in_fisier=0;
        for(int i=0;i<100;i++){
            if(toate_fisiere[i].unic_id==NULL){
              break;
            }
            if(st.st_ino==toate_fisiere[i].unic_id){
                ++apare_in_fisier;
                // fisierul exista => vreau sa vad daca e la fel sau e schimbat
                if(st.st_mtime==toate_fisiere[i].ultimul_timp){
                    printf("fisierul %s nu s a modificat\n",toate_fisiere[i].nume_fisier);
                } else{
                    printf("fisierul %s s a modificat\n",toate_fisiere[i].nume_fisier);
                    update_time_fisier(toate_fisiere,i,fd,st.st_mtime);
                }
            }
        }
        if(apare_in_fisier==0){
            adaugare_fisier(st.st_ino,st.st_mtime,fd,strlen(path),path);
            printf("am adaugat fisierul %s\n",path);
        }
    }    
    
}
int verificare_permisiuni(const char *file_name) {
    int read_verificare = access(file_name,R_OK);
    int write_verificare= access(file_name,W_OK);
    int exec_verificare = access(file_name,X_OK);
    if(read_verificare==-1 && write_verificare==-1 && exec_verificare==-1){
       return 1;  
    } else{
        return 0;
    }
}

void parcurgere(const char *path,int fd,const char *script_path, const char *folder_destinatie)
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
                parcurgere(newPath,fd,script_path,folder_destinatie);
            }
        }

        closedir(director);
    }

    verificare_modificari(path,fd);
    if(strcmp(path,"dir2/file2.txt")==0){
        // printf("am apelat ieee");
        mutare_daca_gasesc_virusi(script_path,path,folder_destinatie);
    }
}

int main(int argc, char *argv[])

{
    if(argc <= 3){
        printf("nu ai adugat toate argumentele mai incearca odata ");
        exit(-1);
    }
    char script_path[20];
    strcpy(script_path,argv[1]);
    char director_mutare[25];
    strcpy(director_mutare,argv[2]);
    int fd;
    // int fd2;
    fd=open("outn.txt",O_RDWR | O_CREAT);
    if (fd < 0) {
        perror("eroare la deschidere");
        exit(1);
   }
    // for (int i = 1; i < argc; i++)
    // {
    //     parcurgere(argv[i],fd);
    // } 
    int status ;
    pid_t pid[argc], wpid;
    for(int i=3;i<argc;i++){
        // pipe(pid[i]);
        pid[i]=fork();
        if(pid[i]==0){
            // child process

            parcurgere(argv[i],fd,script_path,director_mutare);
            exit(0);
        } 
    }

    for(int i=3;i<argc;i++){
       wpid=wait(&status);
       if (WIFEXITED(status)) 
            {
                printf("\nPid-ul procesului pentru ultima modificare %d a ieșit cu codul: %d\n", wpid, WEXITSTATUS(status));
            } 
            else 
            {
                fprintf(stderr, "Eroare la așteptarea procesului\n");
            }
    }
    if (close(fd) < 0) {
        perror("c1");
        exit(1);
    }
    return 0;
}