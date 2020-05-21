#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

struct Server {
  char ip[255];
  int port;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};





void *ThreadServer(void *args) {
  struct Server *to = (struct Server *)args;
   
struct hostent *hostname = gethostbyname(to->ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to->ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to->port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // TODO: for one server
    // parallel between servers
   

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &to->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &to->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &to->mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    // TODO: from one server
    // unite results
    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
     close(sck);
  return (void*)answer;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255
/* Полное имя файла может включать 260 сиволов, которое определенно коснстантой PATH_MAX,  и  тогда взяв в пример Windows Полный путь к файлу находящемся на диске С будет иметь такой вид С:\<256 символов>\0.
И в данном случае можно пользоваться относительными путями как, это происходит в Unix системах.  
Данное количество символов можно обойти подставив префикс "\\?\"
Но в данном случае можно пользоваться только абсолютными путями*/
  char ip[255];
  FILE* file;
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:

	k = atoi(optarg);
        
        // TODO: your code here
   	if(k<=0){
		   printf("number can't be negative \n");		
		   return -1;
		}
	ConvertStringToUI64(optarg, &k);
        break;
      case 1:
         mod = atoi(optarg);
		if(mod<1){
		   	printf("module can't be negative \n");
		   return -1;
		}
	ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
	 
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
	file=fopen(servers,"r");
	 if(file==NULL){

		perror("Can\'t create file\n");
		return 1;
	}
	fclose(file);
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file
  unsigned int servers_num = 1;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  // TODO: delete this and parallel work between servers

servers_num = 0;

char buff[50];
file=fopen(servers,"r");
while(!feof(file))
{
servers_num++;

to=(struct Server*)realloc(to,sizeof(struct Server) * servers_num);

fscanf(file,"%s\n",buff);

int search=0;
for(search; buff[search]!=':';search++);

memcpy(to[servers_num-1].ip, buff, sizeof(char)*(search));
char port[10];
memcpy(port, buff+search+1, sizeof(char)*(search-1));

to[servers_num-1].port=atoi(port);
}
fclose(file);





//localhost - специально зарезервированный в компьютерных сетях доменное имя для частных IP адресов (Его диапазон 127.0.0.0-127.255.255.255)
// 0.0.0.0 не маршуртизируемый мета-адрес. т-е нет конкретного адресата, следоватеьно к нему кроме как вашего собственного компа не подключиться
  // TODO: work continiously, rewrite to make parallel

uint64_t begin = 1;
uint64_t end = k;

int step_factorial_for_server=0;

if( k>servers_num){
step_factorial_for_server=(k/servers_num)-1;
}else{
 step_factorial_for_server=0;
 servers_num=k;
}
pthread_t threads[servers_num];

int beginf=0, endf=begin-1;
for (uint32_t i = 0; i < servers_num; i++) {
        // TODO: parallel somehow	
    beginf=endf+1;
    endf=beginf+step_factorial_for_server;
    if((i+1)==servers_num)
      endf=end;
        to[i].begin = beginf;
        to[i].end = endf;
        to[i].mod = mod;
        if (pthread_create(&threads[i], NULL, ThreadServer,
                           (void *)&to[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

  int total = 1;
  for (int i = 0; i < servers_num; i++) {
   
    int result=0;
    pthread_join(threads[i], (void **)&result);
    total = MultModulo(total, result, mod);
  }
  
  printf("answer: %llu\n", total);
  free(to);


  return 0;
}
