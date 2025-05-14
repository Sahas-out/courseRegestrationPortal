#include <stdio.h>
#include <pthread.h>
#include "server.h"
#include "client_routine.h"
#include "resource_api.h"
struct Server_utils server_util;
void create_client_thread(int client_fd){ 
  pthread_mutex_lock(&server_util.mutex_con_info);
  if(server_util.n_free_thread == 0){
    pthread_mutex_unlock(&server_util.mutex_con_info);
    struct Message msg = {SERVER_FULL,"Try again later server currently full"};
    send(client_fd,&msg,sizeof(struct Message),0);
    return;
  }
  struct Client* free_client; 
  for( int i =0; i< MAX_CONNECTIONS; i++){
    if(server_util.clients[i].status != FREE) continue;
    free_client = &server_util.clients[i];
    break;
  }
  free_client->fd= client_fd;
  free_client->status =  OPENED;
  server_util.n_free_thread--;
  pthread_mutex_unlock(&server_util.mutex_con_info);
  pthread_create(&free_client->thread,NULL,client_routine,free_client);
  pthread_detach(free_client->thread);
  struct Message msg =  {REQ_SUCCESS,"you have been succesfully connected to the server"};
  send(client_fd,&msg,sizeof(struct Message),0);
}
void intialize_server_utils(){
  server_util.n_free_thread = MAX_CONNECTIONS;
  for(int i =0;i<MAX_CONNECTIONS;i++){
    server_util.client_ids[i] = i;
    server_util.clients[i].status = FREE;
    server_util.clients[i].last_msg.req_kind = VOID; 
  }
  pthread_mutex_init(&server_util.mutex_con_info,NULL);
  server_util.fd= socket(AF_INET,SOCK_STREAM,0);
  server_util.addr.sin_family = AF_INET;
  server_util.addr.sin_port = htons(SERVER_PORT); 
  server_util.addr.sin_addr.s_addr = INADDR_ANY;

}
int main(){
  intialize_server_utils();
  intialize_resources();
  if(bind(server_util.fd, (struct sockaddr *)&server_util.addr, sizeof(server_util.addr)) < 0){
    close(server_util.fd);
    return BIND_ERROR;  
  }
  if(listen(server_util.fd, BACKLOG_SIZE) < 0){
    close(server_util.fd);
    return LISTEN_ERROR;
  }
  printf("-----------------\nserver listening now\n\n");
  while(1){
    int client_fd = accept(server_util.fd,NULL,NULL);
    printf("connected with a new client\n");
    create_client_thread(client_fd);
  }
}
