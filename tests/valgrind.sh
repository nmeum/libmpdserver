#!/bin/sh

TESTRUNNER="$(pwd)/../cmd2yaml"

logfile="${TMPDIR:-/tmp}/libmpdserver-valgrind"
trap "rm -f '${logfile}' 2>/dev/null" INT EXIT

for test in *; do
	[ -d "${test}" ] || continue

	name=${test##*/}
	printf "Checking test input '%s': " "${name}"

	valgrind --exit-on-first-error=yes --error-exitcode=42 \
		--leak-check=full --show-leak-kinds=all \
		--log-file="${logfile}" \
		"${TESTRUNNER}" < "${test}/input" >/dev/null

	if [ $? -eq 42 ]; then
		printf "valgrind failed.\n"
		cat "${logfile}"
		exit 1
	fi

	printf "OK.\n"
done
