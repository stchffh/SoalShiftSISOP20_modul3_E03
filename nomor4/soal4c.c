     #include <stdio.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <sys/wait.h>
         
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
