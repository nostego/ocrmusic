#! /bin/bash

bin=./test
red='\033[31m'
white='\033[0m'
green='\033[32m'

total=0
fail=0
maxproc=`grep -c ^processor /proc/cpuinfo`
nproc=0
echo "Launch check on $maxproc cores"
while read line; do
    img=`echo $line|cut -d' ' -f1`
    tests="--rot"
    if (echo "$img"|grep -v 'rot' >/dev/null 2>&1); then
        tests="$tests --straight"
    fi
    for i in $tests; do
        $bin ./corpus/$img "$i" > $img$i.out 2>/dev/null &
        echo "Analyzing $img$i.."
        nproc=$((nproc + 1))
        if [ "$nproc" -ge "$maxproc" ]; then
            wait
            nproc=0
        fi
    done
done < tests.txt 2>/dev/null

function check_that
{
    ref=`echo $line|cut -d' ' -f$(($1 + 1))`
    if (test "$ref" != "-1"); then
        mine=`head -n $1 $img$i.out|tail -n 1`
        verbosename="$img$i"
        verbosename="$verbosename.$2"
        if (test "$ref" != "$mine"); then
            echo -e "${red}KO${white}\t$verbosename\tref<$ref> != <$mine>"
            fail=$((fail + 1))
        else
            echo -e "${green}OK${white}\t$verbosename"
        fi
        total=$((total + 1))
    fi
}

indextest=1
for verbose_test in `cat verbose.txt`; do
    while read line; do
        img=`echo $line|cut -d' ' -f1`
        tests="--rot"
        if (echo "$img"|grep -v 'rot' >/dev/null 2>&1); then
            tests="$tests --straight"
        fi
        for i in $tests; do
            check_that $indextest "$verbose_test"
        done
    done < tests.txt 2>/dev/null
    indextest=$((indextest + 1))
done
rm -f *.out
echo ""
echo -ne "\t"

if (test "$fail" = "0"); then
    echo -e "${green}PASSED $total/$total tests passed${white}"
else
    echo -e "${red}FAILED $fail/$total tests failed${white}"
fi




