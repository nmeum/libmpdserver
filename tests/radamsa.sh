#!/bin/sh

TESTRUNNER="$(pwd)/../cmd2yaml"

testfile="${TMPDIR:-/tmp}/libmpdserver-fuzzing"
trap "rm -f '${testfile}' 2>/dev/null" INT EXIT

while true; do
	find . -name input -a -type f -exec radamsa {} \+ |
		tee "${testfile}" | valgrind --error-exitcode=1 \
		--leak-check=full "${TESTRUNNER}" >/dev/null 2>&1

	ret=$?
	if [ ${ret} -ne 0 -a ${ret} -ne 1 ]; then
		echo "Found input causing crash or leak:" 2>&1
		cat "${testfile}" 2>&1
		exit 1
	fi
done
