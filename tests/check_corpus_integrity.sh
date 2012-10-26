#! /bin/bash

error=0

for file in `ls corpus`; do
    hash=`md5sum corpus/$file|cut -d' ' -f1`
    for file2 in `ls corpus`; do
        hash2=`md5sum corpus/$file2|cut -d' ' -f1`
        if (test "$file" != "$file2"); then
            if (test "$hash" = "$hash2"); then
		error=1
                echo "Error: $file and $file2 are the same images."
            fi
        fi
    done
done

for file in `ls corpus`; do
    if (cat tests.txt|grep "$file" >/dev/null 2>&1); then
	echo -ne ""
    else
	error=1
	echo "Error: $file exist but is not in tests.txt."
    fi
done

cat tests.txt|cut -d' ' -f1 > tmp
for img in `cat tmp`; do
    if (! test -f "corpus/$img"); then
	error=1
	echo "Error: $img in tests.txt but doesn't exist in corpus."
    fi
done
rm -f tmp

if (test "$error" = "0"); then
    echo "Your corpus is fine."
fi

exit $error
