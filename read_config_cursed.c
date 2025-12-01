#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#define FILE_BUFFER_SIZE 1024

//fucking insane stupid magic numbers that correspond to strings backward 
#define PRIVATE_KEY_FILE_STR_HI 0x454c49465f59454b
#define PRIVATE_KEY_FILE_STR_LO 0x5f45544156495250
#define CERTIFICATE_FILE_STR_HI 0x454c49465f455441
#define CERTIFICATE_FILE_STR_LO 0x4349464954524543
#define FULLCHAIN_FILE_STR_HI 0x454c49465f4e4941
#define FULLCHAIN_FILE_STR_LO 0x48434c4c55465f43
#define HOST_NAME_STR_HI 0x454d414e5f54534f
#define HOST_NAME_STR_LO 0x485f4e49414d4f44  
#define DOCUMENT_ROOT_STR_HI 0x524944544f4f525f
#define DOCUMENT_ROOT_STR_LO 0x544e454d55434f44


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
      if(strlen(tok) != 16){
        printf("unknown directive: %s\n",tok);
        return 1;
      }  
      uint64_t lo_bytes = *((uint64_t*)tok);
      uint64_t hi_bytes = *((uint64_t*)(tok+8));
      switch(hi_bytes){
      case PRIVATE_KEY_FILE_STR_HI:
        cfg.private_key_path = val;
        break;
      case CERTIFICATE_FILE_STR_HI:
        cfg.certificate_path = val;
        break;
      case FULLCHAIN_FILE_STR_HI:
        cfg.fullchain_path = val;
        break;
      case HOST_NAME_STR_HI:
        cfg.hostname = val;
        break;
      case DOCUMENT_ROOT_STR_HI:
        cfg.document_root = val;
        break;
      default:
        printf("unknown directive: %s\n", tok);
        return 1;
      }
      
    }
    tok = strtok(NULL, "\n");
  }
}
