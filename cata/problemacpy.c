#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]){
  DIR *dir;
  struct dirent *entry;
  struct stat file_info;
  
  dir = opendir(argv[1]);
  
  if(dir == NULL){
    perror("opendir");
    return 1;
  }

  char *filepath[4096];
  while((entry = readdir(dir)) != NULL){
    sprintf(filepath, "%s/%s", argv[1], entry->d_name);
    int st = stat(filepath, &file_info);
    printf("%s\n", filepath);

    if(st == -1){
      perror("stat");
      exit(1);
    }
    memset(filepath, 0 , strlen(filepath));
  }
  
  closedir(dir);

  return 0;
}