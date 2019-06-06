#!/bin/sh

TESTRUNNER="$(pwd)/../cmd2yaml"

result=0
for test in *; do
	[ -d "${test}" ] || continue

	name=${test##*/}
	printf "Running test case '%s': " "${name}"

	exitstatus=0
	read -r exitstatus < "${test}/exit"

	(cd "${test}" && "${TESTRUNNER}" <input >/dev/null)
	ret=$?

	if [ ${ret} -ne ${exitstatus} ]; then
		printf "FAIL: Expected '%d', got '%d'.\n" \
			"${exitstatus}" "${ret}"

		result=1
		continue
	fi

	printf "OK.\n"
done

exit "${result}"
