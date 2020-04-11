# SoalShiftSISOP20_modul3_E03

- Sitti Chofifah                    05111840000039
- Oktarizka Asviananda Nursanty     05111840000156


### JAWABAN SOAL NOMOR DUA
+ [soal2](https://github.com/stchffh/SoalShiftSISOP20_modul3_E03/tree/master/nomor2)

### PENJELASAN
a) tapclient
```
void *finding()
{
    while(1)
    {
        printf("Waiting for player ...\n");
        sleep(1);
    }
}

void *playing(void *arg)
{
    while(1)
    {
        char ch = getchar();
        if(ch == ' ') 
        {
            printf("hit !!\n");
            send(*(int*) arg, &ch, sizeof(ch), 0);
        }
    }
}
/*int main disini kurang lebih seperti di modul, melakukan pengecekkan apakah client side dan server side terhubung*/
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error\n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    if(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed\n");
        return -1;
    }
    char cmd[1024], cmd2[1024], username[1024], pass[1024], temp[1024]; //declare array
    /*fungsi untuk menampilkan screen 1, yaitu untuk melakukan login dan register*/
    screen1:
    printf("1. Login\n2. Register\nChoices : ");
    scanf("%s", cmd);
    if(strcmp(cmd, "login") == 0)
    {
        strcpy(username, "l ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", username);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        int feedback;
        read(sock, &feedback, sizeof(feedback));
        if(feedback)
        {
            printf("login success\n");
            send(sock, "sukses", 6, 0);
            /*fungsi untuk menampilakn screen 2 yaitu find match dan logout*/
            screen2:
            printf("1. Find Match\n2. Logout\nChoices : ");
            scanf("%s", cmd2);
            /*jika user melakukan logout maka akan kembali ke screen 1*/
            if(strcmp(cmd2, "logout") == 0)
            {
                send(sock, cmd2, strlen(cmd2), 0);
                goto screen1;
            }
            /*jika user memiih find match maka permainan akan dimulai, int start disini akan passing value
            ke server sehingga server bisa memulai game, saat game dimulai kita akan initialisasi health =100
            dan akan berkuran -10 per hit nya yang sudah dijelaskan pada srver side, jika health menyentuh nilai -1
            maka client akan passing value ke server untuk menghentikan permainan, kemudian setelah game berakhir s
            server akan melakukan pengecekkan nilai health sehingga di client side akan terlihat siapa yg menang dan siapa yg
            kalah, health pemain yg kurang dari atau samadengan 0 yg menjadi pemain kalah*/
            else if(strcmp(cmd2, "find") == 0)
            {
                int start;
                send(sock, cmd2, strlen(cmd2), 0);
                pthread_t th;
                pthread_create(&th, NULL, finding, NULL);
                read(sock, &start, sizeof(start));
                pthread_cancel(th);
                printf("Game dimulai silahkan \e[3mtap tap\e[0m secepat mungkin !!\n");
                struct termios prev, cur;
                tcgetattr(0, &prev);
                cur = prev;
                cur.c_lflag &= -ICANON;
                cur.c_lflag &= -ECHO;
                tcsetattr(0, TCSANOW, &cur);
                pthread_t th2;
                pthread_create(&th2, NULL, playing, (void *) &sock);
                int health = 100; //initialisasi health =100
                while(1)
                {
                    read(sock, &health, sizeof(health));
                    if(health == -1) break; //permainan berhenti
                    printf("Health: %d\n", health);
                }
                pthread_cancel(th2);
                bool cek;
                read(sock, &cek, sizeof(cek));
                if(cek) printf("Game berakhir kamu menang\n");
                else printf("Game berakhir kamu kalah\n");
                tcsetattr(0, TCSANOW, &prev);
                goto screen2;
            }
            else
            {
                printf("invalid input\n"); //jika user memasukkan nilai yg salah pada screen 2 maka akan muncul peringatan invalid input
                goto screen2;              
            }
            
        }
        else
        {
            printf("login failed\n"); // jika user memasukkan nilai yg slah pada screen 1 maka akan muncul peringatan login failed
            goto screen1;
        }
    }
    /*fungsi menampilkan usaat melakukan register*/
    else if(strcmp(cmd, "register") == 0)
    {
        strcpy(username, "r ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", temp);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        printf("register success\n");
        goto screen1;
    }
    else
    {
        printf("invalid input\n"); 
        goto screen1;
    }
    return 0;
}
```
b). tapserver
```
typedef struct akun {
    char un[1024];
    char ps[1024];
} akun;

typedef struct client_serv {
    int cid;
    int login;
} client_serv;

typedef struct player {
    int cidp, cide;
    int *healthp, *healthe;
} player;

akun list_akun[100];
bool akun_cek, file_cek, conn_cek;
int conn, akun_n;

/*fungsi untuk mempersiapkan permainan*/
void *ready(void *arg)
{
    client_serv cl = *(client_serv *) arg;
    int cid = cl.cid;
    int log = cl.login;
    if(log)
    {
        screen1:;
        char data[1024], un[1024], ps[1024], kode;
        memset(data, 0, sizeof(data));
        read(cid, data, sizeof(data)); //kode username password
        kode = data[0];
        int id = 0;
        for(int i = 2; i < strlen(data); i++)
        {
            if(data[i] == '\t') break;
            un[id] = data[i];
            id++;
        }
        //un[id] = '\0';
        int id2 = 0;
        id += 3;
        for(int j = id; j < strlen(data); j++)
        {
            ps[id2] = data[j];
            id2++;
        }
        //ps[id2] = '\0';
        //printf("debug: %s - %s\n", un, ps);
        /*untuk melakukan validasi data*/
        if(kode == 'l')
        {
            bool done = 0;
            while(akun_cek);
            for(int i = 0; i < akun_n; i++)
            {
                if((strcmp(un, list_akun[i].un) == 0) && (strcmp(ps, list_akun[i].ps) == 0))
                {
                    done = 1;
                    break;
                }
            }
            send(cid, &done, sizeof(done), 0);
            if(done) printf("Auth success\n");
            else
            {
                printf("Auth Failed\n");
                goto screen1;
            }
        }
        /*fungsi untuk mmembuat dan menyimpan user beserta password di akun.txt*/
        else if(kode == 'r')
        {
            akun_cek = 1;
            akun akun_reg;
            strcpy(akun_reg.un, un);
            strcpy(akun_reg.ps, ps);
            list_akun[akun_n] = akun_reg;
            akun_n++;
            while(file_cek);
            file_cek = 1;
            FILE *fptr = fopen("akun.txt", "a");
            fprintf(fptr, "%s\t%s\n", un, ps);
            fclose(fptr);
            file_cek = 0;
            akun_cek = 0;
            printf("Username\tPassword\n");
            for(int i = 0; i < akun_n; i++)
            {
                akun cur = list_akun[i];
                printf("%s\t%s\n", cur.un, cur.ps);
            }
        }
    }
    log = 1;
    char cmdt;
    read(cid, &cmdt, strlen(&cmdt));
    if(strcmp(&cmdt, "logout") == 0) goto screen1; //jika user melakukan logout maka server passing info ke client untuk kembali ke screen1
    //fungsi jika user melakukan find match
    else if(strcmp(&cmdt, "find") == 0) 
    {
        while(conn_cek);
        conn_cek = 1;
        conn++;
        conn_cek = 0;
        while(conn < 2);
    }
    pthread_exit(0); //thread berhenti
}

/*server menghandle saat permainan berlangsung, jika game sudah dimulai maka health yg 
   awalnya sdh di initalisasi 100 di client side akan berkurang -10 per-hit-nya*/
void *play(void *arg)
{
    player p = *(player *) arg;
    int *he = p.healthe;
    int start = 1;
    send(p.cidp, &start, sizeof(start), 0);
    while(1)
    {
        char ch;
        read(p.cidp, &ch, sizeof(ch));
        *he -= 10;
        send(p.cide, he, sizeof(*he), 0);
    }
}
/*fungsi ini hanya untuk cek apakah server dan client berhubungan, jika ada koneksi maka 
game bisa dimulai*/
/*crop sampai listen itu ta yg exit*/
int main(int argc, char const *argv[])
{
    int c = 0;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(0);
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(0);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(0);
    }

    if(listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(0);
    }
   
    FILE *fp;
    char ch, un[1024], ps[1024];
    int id = 0;
    fp = fopen("akun.txt", "a+");
    if(fp == NULL) exit(0);
    while(fscanf(fp, "%[^\t]\t%[^\n]\n", un, ps) != EOF)
    {
        akun akun_baru;
        strcpy(akun_baru.un, un);
        strcpy(akun_baru.ps, ps);
        list_akun[id] = akun_baru;
        id++;
    }
    fclose(fp);
    akun_n = id;
    akun_cek = 0;
    file_cek = 0;
    conn = 0;
    conn_cek = 0;

    /*setelah akun yang sdh terverifikasi disimpan, selanjutnya jika ada yg bermain dan melakukan login
    server bisa mendeteksi apakah akun yg diinput valid atau tidak*/
    pthread_t th[2];
    int client[2];
    for(int i = 0; i < 2; i++)
    {
        client[i] = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
        if(client[i] < 0)
        {
            perror("can't accept client\n");
            i--;
            continue;
        }
        printf("client accepted\n");
        client_serv client_baru;
        client_baru.cid = client[i];
        client_baru.login = 1;
        pthread_create(&th[i], NULL, ready, (void *) &client_baru);
    }
    /*fungsi yg menangani saat permainann berlangsung, serta menantukan pemain yg mana yg kalah dan yg menang*/
    while(1)
    {
        bool player1_cek = 0;
        bool player2_cek = 0;
        int health1 = 100;
        int health2 = 100;
        player player1, player2;
        player1.cidp = client[0];
        player1.cide = client[1];
        player1.healthp = &health1;
        player1.healthe = &health2;
        player2.cidp = client[1];
        player2.cide = client[0];
        player2.healthp = &health2;
        player2.healthe = &health1;
        pthread_create(&th[0], NULL, play, (void *) &player1);
        pthread_create(&th[1], NULL, play, (void *) &player2);
        while(1) if(health1 <= 0 || health2 <= 0) break;
        pthread_cancel(th[0]);
        pthread_cancel(th[1]);
        int fin = -1, win = 1, lose = 0;
        send(client[0], &fin, sizeof(fin), 0);
        send(client[1], &fin, sizeof(fin), 0);
        conn -= 2;
        if(health1 <= 0)
        {
            send(client[0], &lose, sizeof(lose), 0);
            send(client[1], &win, sizeof(win), 0);
        }
        else if(health2 <= 0)
        {
            send(client[0], &win, sizeof(win), 0);
            send(client[1], &lose, sizeof(lose), 0);
        }
        for(int i = 0; i < 2; i++)
        {
            client_serv p;
            p.cid = client[i];
            p.login = 1;
            pthread_create(&th[i], NULL, ready, (void *) &p);
        }
        pthread_join(th[0], NULL);
        pthread_join(th[1], NULL);
    }
    return 0;
}
```
Catatan : Program yang kami buat hanya dapat dimainkan oleh dua(2) pemain dan tidak bisa lebih.

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
- untuk mengambil extension
```
if(mkdir(folderFinish, 0777) == -1); //bikin folder sesuai ext, jika folder blm ada
   char pathFinish[10000]; //membuat path tujuan yg source nya diambil dari argumen fungsi
   snprintf(pathFinish, 10000, "%s/%s/%s", cwd, folderFinish, filename((char *)arg));
   moveFile((char *)arg, pathFinish);
   return NULL;
}
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
- untuk mengambil extension
```
if(mkdir(folderFinish, 0777) == -1); //bikin folder sesuai ext, jika folder blm ada
   char pathFinish[10000]; //membuat path tujuan yg source nya diambil dari argumen fungsi
   char pathStart[10000]; //membuat path awal yg source nya diambil dari argumen fungsi
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

    /*ambil extension*/                    
    if(mkdir(folderFinish, 0777) == -1); //bikin folder sesuai ext, jika folder blm ada
    char pathFinish[10000]; //membuat path tujuan yg source nya diambil dari argumen fungsi
    char pathStart[10000]; //membuat path awal yg source nya diambil dari argumen fungsi
    snprintf(pathStart, 10000, "%s/%s", tempcwd, (char *)arg);
    snprintf(pathFinish, 10000, "%s/%s/%s", cwd, folderFinish, filename((char *)arg));
    moveFile(pathStart, pathFinish);
    return NULL;
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
#define baca   0               
#define tulis  1               
#define input  0               
#define output 1  

int main(){
int p1,p2,pfd[2];            
if(pipe(pfd)==-1){                              
   perror(" ");
   exit(-1);}
if ((p1=fork())==-1){
     perror(" ");
     exit(-1);}
if (p1!=0){
   if((p2=fork())==-1){
      	perror(" ");
        exit(-1);}
   if(p2!=0){
        close(pfd[baca]);      
        close(pfd[tulis]);     
        wait((int*)0);        
        wait((int*)0);}
    else{
       	close(input);           
        dup(pfd[baca]);      
        close(pfd[baca]);    
        close(pfd[tulis]);   
        execl("/usr/bin/wc","wc","-l",NULL);}}
else{
     close(output);           
     dup(pfd[tulis]);      
     close(pfd[baca]);     
     close(pfd[tulis]);     
     execl("/bin/ls","ls","/home/oktarizka156/",NULL);}
exit (0);}
```
Pertama mendeklarasikan array pipe sebesar 2, lalu mendeklarasikan char yang berisi string perintah. Kemudian memanggil fungsi fork() untuk membuat child process. Setelah itu gunakan fungsi wait() agar child process selesai terlebih dahulu. Pada perintah wc -l gunakan fungsi dup sehingga read end of pipe menjadi stdin, sedangkan pada perintah ls gunakan fungsi dup sehingga write end of pipe menjadi stdout.
