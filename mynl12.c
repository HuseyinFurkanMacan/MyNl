#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define FILE_BUFFER_SIZE 512
#define INIT_BUFF_SIZE 50
#define INC_BUFF_SIZE 10
char *read_line(int fpntr);
int read_char(int fpntr);
int main (int argc, char **argv)
{
  
  int a_flag = 0;//Body Numbering İçin a girilip girilmediğini tutan değişken.
  int regex_flag = 0;//Regex istenip istenmediğini tutan değişken
  //int regex_check=0;
  regex_t regex;
  char *svalue="\t";//Ayraç
  char *bvalue;//Body Numbering girilirse içeriğini tutan değişken.
  int number=1;//Satır numaralandırıcı başlangıç değeri.
  int isregexfailed=0;//Regex kontrol sonucunu tutan değişken
  int compare;
  int index;//Opsiyonlar bittikten sonraki kısım, dosyalar için..
  char *pattern="";//Kullanıcının girdiği regex patterni
  int c;
  char msgbuf[100];//Regex için buffer
  
  opterr = 0;


//OPTIONS SECTION
 while ((c = getopt (argc, argv,"b:s:")) != -1)
    switch (c)
      {
      case 'b':
        bvalue = optarg;
        
        if(*optarg=='a'){a_flag=1;}
        if(*optarg=='p'){pattern=optarg+1;regex_flag=1;}
        break;
      case 's':
      	svalue = optarg;
      	break;
      case '?':
        if (optopt == 'b')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }//fprintf(stdout,"argument: %s" , argv[optind]);
//fprintf(stdout,"\n %d",argc);



////////////////////////////////////////////////
char *file_pathname, *reader;
int fpntr;
index=optind;
if(!argv[index])argc+=1;

for (index; index < argc; index++){

    //Girilen dosya yoksa dosya kontrolüne girmez ve devam eder.
   if(argv[index]){

   file_pathname=argv[index];
   
   if ((fpntr = open(file_pathname, O_RDONLY)) == -1) {
    fprintf(stderr,"Error opening file %s: %s\n",file_pathname, strerror(errno));
    return EXIT_FAILURE;
  }
  //else{printf("Dosya Açıldı\n");}

   }
  else{
    fpntr=stdin;
  }
  //Dosyadan Satır Al.
  while((reader=read_line(fpntr))!=EOF){
  
//Boş satır kontrolü ve a_flag kontrolü

  if(strlen(reader)==0 && a_flag==0){
  printf("%s",reader);

  continue;
  }
  
  if(regex_flag==1){
  isregexfailed=0;
  compare=regcomp(&regex,pattern,0);
  if(compare){
  fprintf(stderr,"Compiling failed\n");
  }
  else{
  compare=regexec(&regex,reader,0,NULL,0);
  if(!compare){
  printf("Eşleşme Başarılı\n");
  }
  else if(compare==REG_NOMATCH){
  printf("Eşleşme Yok.\n");
  isregexfailed=1;
  
  }
  
  else{
  regerror(compare,&regex,msgbuf,sizeof(msgbuf));
  fprintf(stderr,"Regex match failed: %s\n",msgbuf);
  }
  
  }//else end
   regfree(&regex);
   if(isregexfailed){continue;}
  }//regex end
   
   
   	printf("%6d%s%s",number++,svalue,reader);
   }
  
   
   if (reader == EOF) {
    close(fpntr);
    
    }
   
   }


  return EXIT_SUCCESS;
}


char *read_line(int fpntr){//Satır okuyucu fonksiyon
  static char line_buff[101];
  for(int m=0;m<101;m++){
    line_buff[m]=NULL;
  }
  int line=0;
  
  while((line_buff[line]=read_char(fpntr))!='\n' && line_buff[line]!=EOF){
  line++;
  }


  if((line_buff[line])==EOF)return EOF;
    line_buff[line]=='\0';

    return line_buff;
    
}

int read_char(int fpntr){//Bir önbelleğe karakter doldurup read_line a döndüren fonksiyon.
	static char file_buff[512];
  static int point=0;
  static int firstflag=1;
  int temp=0;
  static int nread=0;
  
  if(point==nread){
  if((nread=read(fpntr,&file_buff,512*sizeof(char)))<0){fprintf(stderr,"Problem reading from file");exit(EXIT_FAILURE); } 
  

  point=0;
  if(nread==0)return EOF;
  }

  if(nread>0 && point<nread || firstflag==1){
    
    firstflag=0;
    return file_buff[point++];
  }
 
 
}



