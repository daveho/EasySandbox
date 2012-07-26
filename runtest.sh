#! /bin/bash

testexe=$1

echo -n "Executing ${testexe}..."

./${testexe} > /tmp/test$$.out
testexe_rc=$?
diff /tmp/test$$.out oracle/${testexe}.out
diff_rc=$?

actual_output=`cat /tmp/test$$.out`
rm -f /tmp/test$$.out
if [ $diff_rc != 0 ]; then
	echo "failed (output mismatch, expected [`cat oracle/${testexe}.out`], got [${actual_output}])"
	exit 1
fi

expected_rc=`cat oracle/${testexe}.exit`
if [ $testexe_rc != $expected_rc ]; then
	echo "failed (exit code mismatch, expected ${expected_rc}, got ${testexe_rc})"
	exit 1
fi

echo "passed!"
exit 0
