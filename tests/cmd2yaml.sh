#!/bin/sh

TESTRUNNER="$(pwd)/../cmd2yaml"

dir=${TMPDIR:-/tmp}/libmpdserver
trap "rm -rf '${dir}' 2>/dev/null" INT EXIT
mkdir -p "${dir}"

result=0
for test in *; do
	[ -d "${test}" ] || continue

	name=${test##*/}
	printf "Running test case '%s': " "${name}"

	(cd "${test}" && "${TESTRUNNER}" <input >"${dir}/out.pre")
	ret=$?

	test -r "${test}/output.yml"
	expected=$?

	if [ ${ret} -ne ${expected} ]; then
		printf "FAIL: Expected '%d', got '%d'.\n" \
			"${expected}" "${ret}"

		result=1
		continue
	fi

	sed 's/[ \t]*$//' < "${dir}/out.pre" > "${dir}/out"
	if [ ${ret} -eq 0 ] && ! cmp -s "${dir}/out" "${test}/output.yml"; then
		printf "FAIL: Output didn't match.\n"
		diff -u "${dir}/out" "${test}/output.yml"

		result=1
		continue
	fi

	printf "OK.\n"
done

exit "${result}"
