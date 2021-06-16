// Work queue example using wsummary

#include "work_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

void run_task(int tasks, double tasktime, double posttime)
{
	time_t my_t;
	my_t = time(NULL);

	struct work_queue *q;
	struct work_queue_task *t;
	int taskid;
	int i;
	char *sleep_path = "/bin/sleep";
	q = work_queue_create(WORK_QUEUE_DEFAULT_PORT);
	printf("listening on port %d...\n", work_queue_port(q));
	
	for(i = 1; i < tasks; i++) {
		char command[128];
		sprintf(command, "./sleep %.0lf", tasktime);

		t = work_queue_task_create(command);

		work_queue_task_specify_file(t, sleep_path, "sleep", WORK_QUEUE_INPUT, WORK_QUEUE_CACHE);
		//work_queue_task_specify_file(t, "/tmp/dsimone2/bigfile.txt", "bigfile.txt", WORK_QUEUE_INPUT, WORK_QUEUE_NOCACHE);
		taskid = work_queue_submit(q, t);

		printf("submitted task (id# %d): %s\n", taskid, t->command_line);
	}

	printf("waiting for tasks to complete...\n");
	printf("bro");
	while(!work_queue_empty(q)) {
		printf("yo");
		t = work_queue_wait(q, 1);
		if(t) {
			printf("task (id# %d) complete: %s (return code %d)\n", t->taskid, t->command_line, t->return_status);	
			work_queue_task_delete(t);
			//usleep(posttime * 1000);
		}
	}
printf("slo");
	printf("all tasks complete!\n");

	FILE *fp = fopen("log2.txt", "a");
	fprintf(fp, "Final time for %d tasks with %d sleep time with %d postprocessing: %d\n",
	 		tasks, tasktime, posttime, time(NULL) - my_t );
	work_queue_delete(q);
}

int main(int argc, char *argv[])
{
	
	run_task(10, 1, 0);
	run_task(10, 0.1, 0.1);
	run_task(100, 0.1, 0);
	run_task(100, 0.1, 0.1);
	//run_task(1000, 1, 0);
	//run_task(10, 1, 1);
	//run_task(100, 1, 10);
	//run_task(10, 10, 0);
	//run_task(100, 10, 0);
	//run_task(1000, 10, 0);
	//run_task(10, 1, 10);
	return 0;
}

/* vim: set noexpandtab tabstop=4: */
