#! /bin/bash

testexe=$1

echo -n "Executing ${testexe}..."

input=oracle/${testexe}.in
actual=/tmp/actual$$
expected=oracle/${testexe}.out

if [ ! -r oracle/${testexe}.in ]; then
	# Test does not expect input
	LD_PRELOAD=./EasySandbox.so ./${testexe} > ${actual}
	testexe_rc=$?
else
	# Test expects input from stdin
	LD_PRELOAD=./EasySandbox.so ./${testexe} < ${input} > ${actual}
	testexe_rc=$?
fi
diff ${actual} ${expected}
diff_rc=$?

actual_output=`cat ${actual}`
rm -f ${actual}
if [ $diff_rc != 0 ]; then
	echo "failed (output mismatch, expected [`cat ${expected}`], got [${actual_output}])"
	exit 1
fi

expected_rc=`cat oracle/${testexe}.exit`
if [ $testexe_rc != $expected_rc ]; then
	echo "failed (exit code mismatch, expected ${expected_rc}, got ${testexe_rc})"
	exit 1
fi

echo "passed!"
exit 0
