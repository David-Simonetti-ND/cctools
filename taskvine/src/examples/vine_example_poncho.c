/*
Copyright (C) 2022- The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file COPYING for details.
*/

/*
An example of a task using a minitask (vine_declare_poncho) to unpack a dependency before using it.
*/

#include "taskvine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	struct vine_manager *m;
	struct vine_task *t;
	int i;

	// runtime logs will be written to vine_example_poncho_worker_info/%Y-%m-%dT%H:%M:%S
	vine_set_runtime_info_path("vine_example_poncho_worker_info");

	m = vine_create(VINE_DEFAULT_PORT);
	if(!m) {
		printf("couldn't create manager: %s\n", strerror(errno));
		return 1;
	}
	printf("listening on port %d...\n", vine_port(m));

	struct vine_file *script = vine_declare_file(m, "python_example.py");

	struct vine_file *poncho_tarball = vine_declare_file(m, "package.tar.gz");
	struct vine_file *poncho_expansion = vine_declare_poncho(m, poncho_tarball);

	for(i=0;i<5;i++) {

		struct vine_task *task = vine_task_create("python python_example.py");

		vine_task_add_input(task, script, "python_example.py", VINE_CACHE);
		vine_task_add_input(task, poncho_expansion, "package", VINE_CACHE);

		int task_id = vine_submit(m, task);
		printf("submitted task (id# %d): %s\n", task_id, vine_task_get_command(task) );
	}

	printf("waiting for tasks to complete...\n");

	while(!vine_empty(m)) {
		t  = vine_wait(m, 5);
		if(t) {
			vine_result_t r = vine_task_get_result(t);
			int id = vine_task_get_id(t);

			if(r==VINE_RESULT_SUCCESS) {
				printf("task %d output: %s\n",id,vine_task_get_stdout(t));
			} else {
				printf("task %d failed: %s\n",id,vine_result_string(r));
			}
			vine_task_delete(t);
		}
	}

	printf("all tasks complete!\n");

	vine_delete(m);

	return 0;
}

/* vim: set noexpandtab tabstop=4: */
