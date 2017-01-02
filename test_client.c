/* Copyright 2008 The Android Open Source Project
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/types.h>
#include <stdbool.h>
#include <string.h>
#include <private/android_filesystem_config.h>
#include "binder.h"
#include "test_server.h"


 struct binder_state *g_bs;
 uint32_t svcmgr = BINDER_SERVICE_MANAGER;
 uint32_t g_handle;
 
uint32_t svcmgr_lookup(struct binder_state *bs, uint32_t target, const char *name)
{
    uint32_t handle;
    unsigned iodata[512/4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_string16_x(&msg, name);

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_CHECK_SERVICE))
        return 0;

    handle = bio_get_ref(&reply);

    if (handle)
        binder_acquire(bs, handle);

    binder_done(bs, &msg, &reply);

    return handle;
}

/**
  * ./test_client hello
  *  ./test_client hello some
  */

void say_hello()
{

int status;
    unsigned iodata[512/4];
    struct binder_io msg, reply;

	/*构造Binder_io*/

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
       
      if (binder_call(g_bs,&msg, &reply, g_handle,HELLO_SVR_CMD_SAYHELLO))
	  	return ;
	   binder_done(g_bs, &msg, &reply);
}

int say_hello_to(char *name)
{
	int ret;
   	unsigned iodata[512/4];
    	struct binder_io msg, reply;

	/*构造Binder_io*/

    	bio_init(&msg, iodata, sizeof(iodata), 4);
    	bio_put_uint32(&msg, 0);  // strict mode header
    	bio_put_string16_x(&msg, name);

		
      	if (binder_call(g_bs,&msg, &reply, g_handle,HELLO_SVR_CMD_SAYHELLO_TO))
	  	return ;

 	ret = bio_get_uint32(&reply);
	
	binder_done(g_bs, &msg, &reply);

	return ret;
}
int main(int argc, char **argv)
{
    int fd,ret;

    g_bs = binder_open(128*1024);
    if (!g_bs) {
        fprintf(stderr, "failed to open binder driver\n");
        return -1;
    }

	/*get service*/
	g_handle = 	svcmgr_lookup(g_bs, svcmgr, "hello");
	if(!g_handle){
        fprintf(stderr, "failed to get esrvice\n");
        return -1;
    }
	/*send data to server */
	if(argc==2) {
		say_hello();
	} else if(argc ==3) {
		ret = say_hello_to(argv[2]);
		printf("say_hello_to get ret = %d\n", ret);
	}
	binder_release(g_bs, g_handle);
    return 0;
}
