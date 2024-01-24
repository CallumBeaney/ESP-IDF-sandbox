#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

/// Illustrates basic POSIX directory/filehandling. 
/// Not the same on ESP chips; for contradistinctive reference.

#define REPEAT_COUNT 3
#define MAX_PATH_LENGTH 100 // Define maximum length for the path
#define MOUNTPOINT "/sdcard"
#define DIR_PREFIX "/capture_"
#define FILENAME "hallo.txt"

char* getFolderPath(const char* mountPoint, const char* folderName, int folderNum);
char* getFilePath(const char* folderPath, const char* fileName);
void makeDirectory(const char* folderPath);
void testIO(const char* path);
void mainTask(int folderNum);



int main(void) 
{
  int folderNum = 0;
  srand((unsigned int)time(NULL)); // seed generator with current time

  // make base-level directory
  char* SDpath = (char*)malloc(MAX_PATH_LENGTH * sizeof(char));
  snprintf(SDpath, MAX_PATH_LENGTH, ".%s", MOUNTPOINT); // note `.`
  makeDirectory(SDpath);

  for (int i = 0; i < REPEAT_COUNT; i++, folderNum++) mainTask(folderNum);
}



void mainTask(int folderNum) 
{
  char* folderPath = getFolderPath(MOUNTPOINT, DIR_PREFIX, folderNum);
  char* filePath = getFilePath(folderPath, FILENAME);

  makeDirectory(folderPath);
  testIO(filePath);

  free(folderPath);
  free(filePath);
}


void makeDirectory(const char* folderPath) 
{
  int res = mkdir(folderPath, 0777);
  if (res == -1 && errno != EEXIST) {
      printf("Failed to create folder (%s)\n", strerror(errno));
      return;
  } else {
      printf("Folder created successfully\n");
  }
}


char* getFolderPath(const char* mountPoint, const char* folderName, int folderNum) 
{
  int randomNumber = rand() % 100000;

  char* folderPath = (char*)malloc(MAX_PATH_LENGTH * sizeof(char));
  snprintf(folderPath, MAX_PATH_LENGTH, ".%s%s%d_%d", mountPoint, folderName, folderNum, randomNumber);
  printf("Complete folder path: %s\n", folderPath);
  return folderPath;
}


char* getFilePath(const char* folderPath, const char* fileName) 
{
  char* completeFilePath = (char*)malloc(MAX_PATH_LENGTH * sizeof(char));
  snprintf(completeFilePath, MAX_PATH_LENGTH, "%s/%s", folderPath, fileName);

  printf("Complete file path: %s\n", completeFilePath);
  return completeFilePath;
}


void testIO(const char* path) 
{
  FILE *fp = fopen(path, "wb");
      
  if (fp == NULL) {
    printf("fp is null\n");
    return;
  }

  fprintf(fp, "hello, my name is bob!\n"); 
  fprintf(fp, "hello, my name is also bob!\n");
  fclose(fp);

  fp = fopen(path, "r");
  if (fp == NULL) {
    printf("fp is null 2\n");
    return;
  }

  char buffer[100];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("%s", buffer);
  }

  fclose(fp);
}
