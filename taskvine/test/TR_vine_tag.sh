#!/bin/sh
set -e

. ../../dttools/test/test_runner_common.sh

import_config_val CCTOOLS_PYTHON_TEST_EXEC
import_config_val CCTOOLS_PYTHON_TEST_DIR

export PYTHONPATH=$(pwd)/../src/bindings/${CCTOOLS_PYTHON_TEST_DIR}:$PYTHONPATH

STATUS_FILE=vine.status
PORT_FILE=vine.port


check_needed()
{
	[ -n "${CCTOOLS_PYTHON_TEST_EXEC}" ] || return 1
}

prepare()
{
	rm -f $STATUS_FILE
	rm -f $PORT_FILE

	return 0
}

run()
{
	# send vine to the background, saving its exit status.
	(${CCTOOLS_PYTHON_TEST_EXEC} vine_test_tag.py $PORT_FILE; echo $? > $STATUS_FILE) &

	# wait at most 15 seconds for vine to find a port.
	wait_for_file_creation $PORT_FILE 15

	run_ds_worker $PORT_FILE worker.log

	# wait for vine to exit.
	wait_for_file_creation $STATUS_FILE 15

	# retrieve vine exit status
	status=$(cat $STATUS_FILE)
	if [ $status -ne 0 ]
	then
		exit 1
	fi

	exit 0
}

clean()
{
	rm -f $STATUS_FILE
	rm -f $PORT_FILE

	rm -rf vine-run-info

	exit 0
}


dispatch "$@"

# vim: set noexpandtab tabstop=4:
