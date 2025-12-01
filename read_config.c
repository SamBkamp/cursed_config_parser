#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#define FILE_BUFFER_SIZE 1024


typedef struct{
  char *private_key_path;
  char *certificate_path;
  char *fullchain_path;
  char *hostname;
  char *document_root;
}config;

//takes a line src and returns a pointer to the value after the '='. puts a null terminator at the start of delimeter. Accounts for spaces either side of equals.
//eg: field = value -> field\0= value (where val points to the first char of the value ie. 'v' in this case)
//or filed=value -> field\0value 
char* split_line(char *src){
  char *line_end = src + strlen(src); //points to null terminator
  char *equals_pos;
  for(equals_pos = src; equals_pos < line_end && *equals_pos != '='; equals_pos++){}
  if(equals_pos == line_end)
    return equals_pos;
  if(*(equals_pos-1) == ' ')
    *(equals_pos-1) = 0;
  else
    *equals_pos = 0;
  
  equals_pos++;
  while(*equals_pos == ' ')
    equals_pos++;
  return equals_pos;
}

int main(int argc, char* argv[]){
  config cfg = {0};
  char file_data[FILE_BUFFER_SIZE];
  struct stat sb;
  ssize_t bytes_read;
  int conf_fd = open("config.pws", O_RDONLY);
  
  if(conf_fd<0) return 1;
  if(fstat(conf_fd, &sb)<0 || sb.st_size >= FILE_BUFFER_SIZE) return 1;

  bytes_read = read(conf_fd, file_data, FILE_BUFFER_SIZE);
  if(bytes_read < 0) return 1;
  
  file_data[bytes_read] = 0;  
  char *tok = strtok(file_data, "\n");
  while(tok != NULL){
    char *val = split_line(tok);
    if(*tok != '#' && *val != 0){
      if(strcmp(tok, "PRIVATE_KEY_FILE")==0)
        cfg.private_key_path = val;
      else if(strcmp(tok, "CERTIFICATE_FILE")==0)
        cfg.certificate_path = val;
      else if(strcmp(tok, "C_FULLCHAIN_FILE")==0)
        cfg.fullchain_path = val;
      else if(strcmp(tok, "DOMAIN_HOST_NAME")==0)
        cfg.hostname = val;
      else if(strcmp(tok, "DOCUMENT_ROOTDIR")==0)
        cfg.document_root = val;
      else{
        printf("unknown directive %s\n", tok);
        return 1;
      }
    }
    tok = strtok(NULL, "\n");
  }
  printf("priv key path: %s\n", cfg.private_key_path);
  printf("cert path: %s\n", cfg.certificate_path);
  printf("fullchain path: %s\n", cfg.fullchain_path);
  printf("hostname: %s\n", cfg.hostname);
  printf("document_root: %s\n", cfg.document_root);
}
