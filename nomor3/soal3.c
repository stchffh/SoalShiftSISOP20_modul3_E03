#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<dirent.h>

pthread_t tid[100000];

char *cwd;
char *tempcwd;

void moveFile(char start[], char finish[]){
int cek;
FILE *fp1, *fp2;
fp1 = fopen(start, "r");
fp2 = fopen(finish, "w");
    
if (!fp1) 
{
  printf("gabisa\n");
  fclose(fp2);
  return;
}
if (!fp2) 
{
  printf("gabisa\n");
  return ;
}

while ((cek=fgetc(fp1))!= EOF) 
{
  fputc(cek, fp2);
}

fclose(fp1);
fclose(fp2);

remove(start);
return ;
}

char* filename(char string[])
{
char* cekk;
char* hasil;
cekk = strchr(string,'/');
if(cekk == NULL)
return string;
    
while (cekk != NULL) 
{
  hasil = cekk+1;
  cekk = strchr(cekk+1,'/');
}
  return hasil;
}

char* ext(char string[])
{
char* cekk = filename(string);
char* hasil = strchr(cekk, '.');
   
if(hasil == NULL)
return NULL;
else return (hasil+1);
}

void* command_f(void *arg)
{
pthread_t id=pthread_self();
char *folderext;
char folderFinish[10000];
folderext = ext((char *)arg);
if(folderext == NULL) strcpy(folderFinish, "Unknown");
else
{
  strcpy(folderFinish, folderext);
  int x;
  for(x=0; x<strlen(folderFinish); x++)
  if(folderFinish[x] > 64 && folderFinish[x] < 91)
  folderFinish[x] += 32;
}
/*ambil extension*/      
if(mkdir(folderFinish, 0777) == -1); //bikin folder sesuai ext, jika folder blm ada
   char pathFinish[10000]; //membuat path tujuan yg source nya diambil dari argumen fungsi
   snprintf(pathFinish, 10000, "%s/%s/%s", cwd, folderFinish, filename((char *)arg));
   moveFile((char *)arg, pathFinish);
   return NULL;
}

void* command_star(void *arg)
{
pthread_t id=pthread_self();
char *folderext;
char folderFinish[10000];
folderext = ext((char *)arg);
if(folderext == NULL) strcpy(folderFinish, "Unknown");
else
{
  strcpy(folderFinish, folderext);
  int x;
  for(x=0; x<strlen(folderFinish); x++)
  if(folderFinish[x] > 64 && folderFinish[x] < 91)
  folderFinish[x] += 32;
}
            
if(mkdir(folderFinish, 0777) == -1);
   char pathFinish[10000];
   char pathStart[10000];
   snprintf(pathStart, 10000, "%s/%s", cwd, (char *)arg);
   snprintf(pathFinish, 10000, "%s/%s/%s", cwd, folderFinish, filename((char *)arg));
   moveFile(pathStart, pathFinish);
   return NULL;
}

void* command_d(void *arg)
{
   pthread_t id=pthread_self();
   char *folderext;
   char folderFinish[10000];
   folderext = ext((char *)arg);
   if(folderext == NULL) strcpy(folderFinish, "Unknown");
   else
{
    strcpy(folderFinish, folderext);
    int x;
    for(x=0; x<strlen(folderFinish); x++)
    if(folderFinish[x] > 64 && folderFinish[x] < 91)
    folderFinish[x] += 32;
}
            
    if(mkdir(folderFinish, 0777) == -1);
    char pathFinish[10000];
    char pathStart[10000];
    snprintf(pathStart, 10000, "%s/%s", tempcwd, (char *)arg);
    snprintf(pathFinish, 10000, "%s/%s/%s", cwd, folderFinish, filename((char *)arg));
    moveFile(pathStart, pathFinish);
    return NULL;
}

int main(int argc, char **argv)
{
//buat dapetin cwd program ketika dijalanin
char save[1000];
cwd = getcwd(save, 1000);

int error;
int i=2;
int j;
  
if(!strcmp(argv[1], "-f"))
{
  while(argv[i] != NULL)
  {
    //cek error
    error=pthread_create(&(tid[i-2]),NULL,&command_f,(void *)argv[i]);
    if(error!=0) printf("mon maap,coba lagi\n");
    else printf("yuhu\n");
    i++;
   }
    //nunggu semua thread kelar kerja
   for(j=0; j<(i-1); j++) pthread_join(tid[j],NULL);
} 
else if(!strcmp(argv[1], "*")) 
{
  //cek direktori
  DIR *dir;
  struct dirent *tmp;
  struct stat ffile;
  dir = opendir(".");
  if(dir)
  {
    while((tmp = readdir(dir)) != NULL)
    {
      if (stat(tmp->d_name, &ffile) < 0); //meriksa file/direktori
      else if (!S_ISDIR(ffile.st_mode))
      {
        error=pthread_create(&(tid[i-2]),NULL,&command_star,(void *)tmp->d_name); //error handling 
        if(error!=0) printf("mon maap,coba lagi\n");
        else printf("yuhu\n");
        i++;
      } 
       else;
     }
       for(j=0; j<(i-1); j++) pthread_join(tid[j],NULL); //nunggu thread kelar
   }
} 
else if(!strcmp(argv[1], "-d")) 
{
  //ubah cwd sesuai argumen
  chdir(argv[2]);
  tempcwd = getcwd(save, 1000);
  DIR *dir;
  struct dirent *tmp;
  struct stat ffile;
  dir = opendir(".");
  if(dir) //initialisasi cek direktori
  {
   while((tmp = readdir(dir)) != NULL) //cek file ada ato ga
   {
    if (stat(tmp->d_name, &ffile) < 0); //cek file/direktori
    else if (!S_ISDIR(ffile.st_mode))
    {
      error=pthread_create(&(tid[i-2]),NULL,&command_d,(void *)tmp->d_name); //error handling
      if(error!=0) printf("mon maap, coba lagi\n");
      i++;
    } 
    else;
    }
    for(j=0; j<(i-1); j++) pthread_join(tid[j],NULL);//nunggu kerja thread kelar
    }
   }
return 0;
}

