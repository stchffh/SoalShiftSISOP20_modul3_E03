# SoalShiftSISOP20_modul3_E03

- Sitti Chofifah                    05111840000039
- Oktarizka Asviananda Nursanty     05111840000156

### JAWABAN SOAL NOMOR TIGA
+ [soal3.c](https://github.com/stchffh/SoalShiftSISOP20_modul3_E03/blob/master/nomor3/soal3.c)

### PENJELASAN

- char untuk coworking directory dan temporary cwd
```
char *cwd;
char *tempcwd;
```
- fungsi untuk memindahkan file untuk semua command
```
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
```
- fungsi untuk mendapatkan nama file
```
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
```
- fungsi untuk mendapatkan extension
```
char* ext(char string[])
{
char* cekk = filename(string);
char* hasil = strchr(cekk, '.');
   
if(hasil == NULL)
return NULL;
else return (hasil+1);
}
```
- fungsi untuk menjalankan perintah -f
```
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
```
- membuat folder sesuai extension serta jika folder belum ada
```
if(mkdir(folderFinish, 0777) == -1); 
```
- membuat path tujuan yg source nya diambil dari argumen fungsi
```
char pathFinish[10000]; 
```
- fungsi untuk menjalankan perintah *
```
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
```
- fungsi untuk menjalankan perintah -d
```
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


}
```
- program utama
```
int main(int argc, char **argv)
{
//buat dapetin cwd program ketika dijalanin
char save[1000];
cwd = getcwd(save, 1000);

int error; //untuk handling error
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
      if (stat(tmp->d_name, &ffile) < 0); //meriksa objek yg akan dikategorikan berupa file atau direktori
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
    if (stat(tmp->d_name, &ffile) < 0); //meriksa objek yg akan dikategorikan berupa file atau direktori
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

```
Jadi jika disimpulkan codingan dari no 3 ini akan mengkategorian file/directory sesuai command , disini thread yg terbentuk sesuai dengan command, apabila command -f maka thread yg akan terbentuk sebanyak argumen nama file yg diinput setelahnya, jika command -d maka thread akan terbentuk sebanyak directory yg akan dikategorisasikan, jika command * maka thread yg akan terbentuk sebanding dengan banyak file yg ada di cwd.

### JAWABAN SOAL NOMOR EMPAT
+ [soal4a.c](https://github.com/stchffh/SoalShiftSISOP20_modul3_E03/blob/master/nomor4/soal4a.c)
+ [soal4b.c](https://github.com/stchffh/SoalShiftSISOP20_modul3_E03/blob/master/nomor4/soal4b.c)
+ [soal4c.c](https://github.com/stchffh/SoalShiftSISOP20_modul3_E03/blob/master/nomor4/soal4c.c)

### PENJELASAN
a). SOAL 4A
- Untuk Mendeklarasikan matriks dan isi matriks
```
#define M 4
#define K 2
#define N 5
#define NUM_THREADS M * N
int (*value)[10];

int A[M][K] = {{0,1},
               {2,3},
               {4,5},
               {6,7}
              };
int B[K][N] = {{11,12,13,14,15}, 
               {16,17,18,19,10}
              };
```
- Struct untuk passing data ke threads
```
struct v
{
	int i; /* row */
	int j; /* column */
};
```
- `void *runner(void *ptr);` Untuk membuat thread

- Membuat shared memory
```
int main(int argc, char **argv)
{
	int i, j;
	int thread_counter = 0;
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[10][10]), IPC_CREAT | 0666);
    value = shmat(shmid, 0, 0);
    
	
	pthread_t workers[NUM_THREADS];
  
	```
	- Membuat M*N bekerja di thread
  ```
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++) 
		{
			struct v *data = (struct v *) malloc(sizeof(struct v));
			data->i = i;
			data->j = j;
			//Membuat thread dan mempassing data sebagai parameter//
			pthread_create(&workers[thread_counter], NULL, runner, data);
			pthread_join(workers[thread_counter], NULL);
			thread_counter++;
		}
	}
	``` 
  - Menunggu thread selesai dikalikan
  ```
	for (i = 0; i < NUM_THREADS; i++)
	{
	    pthread_join(workers[i], NULL);
	}
	
	for(i = 0; i < M; i++)
	{ 
		for(j = 0; j < N; j++)
		{ 
			printf("%d\t", value[i][j]);
		}
		printf("\n");
	}
	return 0;
}
```
- Menghitung perkalian matriks
```
void *runner(void *ptr)
{
	struct v *data = ptr;
	int i, sum = 0;
	
	for(i = 0; i < K; i++)
	{	
		sum += A[data->i][i] * B[i][data->j];
	}
	
	value[data->i][data->j] = sum;
	pthread_exit(0);
}
```
b). SOAL 4B
- Untuk join thread
```
void pthread_exit(void *rval_ptr);
int pthread_join(pthread_t thread, void **rval_ptr);
pthread_t thread1;

void *buat_factorial();
unsigned long long hasil_factorial[4][5];
```
= Membuat shared memory
```
    key_t key = 1234;
    int (*value)[10];
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, 0, 0);
```
- Menampilkan matriks
```
    int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			printf("%d\t", value[i][j]);
		}
		printf("\n");
	}

	printf("\n");
 ```
- Join thread untuk menampilkan hasil faktorial
```
	int buat_thread;
	buat_thread = pthread_create(&thread1, NULL, buat_factorial, NULL);
	pthread_join(thread1,NULL);
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			printf("%llu\t", hasil_factorial[i][j]);
		}
		printf("\n");
	}

	exit (EXIT_SUCCESS);
}
```
- Membuat fungsi thread faktorial
```
void *buat_factorial()
{
    key_t key = 1234;
    int (*value)[10];
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, 0, 0);

	int i,j,k;
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			unsigned long long f=1;
			for(k=1;k<=value[i][j];k++)
			{
				f= f+k;
				hasil_factorial[i][j] = f-1;
			}
		}
	}
}
```

c). SOAL 4C
	```
     #define baca   0   //membaca ujung pipe          
     #define tulis  1   //menulis ujung pipe           
     #define input  0   //fd standar input
     #define output 1   //fd standar output

     int main(){
        int p1,p2,pfd[2];

        //Membuat pipe           
        if(pipe(pfd)==-1){                              
            perror(" ");
            exit(-1);}

        //Membuat proses anak pertama
        if ((p1=fork())==-1){
            perror(" ");
            exit(-1);}

        //Didalam parent
        if (p1!=0){
            //Membuat proses anak kedua
            if((p2=fork())==-1){
                perror(" ");
                exit(-1);}
            //Masih didalam parent
            if(p2!=0){
                close(pfd[baca]);  //menutup pipe di parent   
                close(pfd[tulis]); //menyimpan file descriptor   
                wait((int*)0);     //menunggu proses anak mati   
                wait((int*)0);}
            //Didalam proses anak kedua//    
            else{
                close(input);  //menutup standar input         
                dup(pfd[baca]); //membaca akhir pipe agar menjadi standar input  
                close(pfd[baca]);  //menghapus I/O yang tidak diperlukan
                close(pfd[tulis]);   
                 execl("/usr/bin/wc","wc","-l",NULL);}} //menghitung banyak file dan folder
        //Didalam proses anak pertama         
        else{
            close(output); //menutup standar output           
            dup(pfd[tulis]);  //menulis akhir pipe agar menjadi standar output    
            close(pfd[baca]);  //menghapus I/O yang tidak diperlukan   
            close(pfd[tulis]);     
            execl("/bin/ls","ls","/home/oktarizka156/",NULL);} //mengeluarkan hasil banyak file dan folder pada direktori
        exit (0);}
	```
