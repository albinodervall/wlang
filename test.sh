#!/bin/zsh
if sh build.sh; then
    EXECUTABLE_EXTENSION=""
    if [[ "$OSTYPE" == "msys" ]]; then
        EXECUTABLE_EXTENSION=".exe"
    fi

    if [[ ! -z $1 ]]; then
        output_file=${1##*/}
        output_file=${output_file%.w}

        if [ "$2" = "d" ]; then
            if lldb "./bin/wlang${EXECUTABLE_EXTENSION}" -- -v test/$1 -O bin; then
                ./bin/$output_file
            fi
        else
            if "./bin/wlang${EXECUTABLE_EXTENSION}" -v -p test/$1 -O bin; then
                if [ "$2" = "dt" ]; then
                    lldb ./bin/$output_file
                else
                    ./bin/$output_file

                    if [ "$2" = "asm" ]; then
                        objdump -disassemble -x86-asm-syntax=intel bin/$output_file
                    fi
                fi
            fi
        fi
    else
        total=0
        passed=0

        failures=()

        for file in test/*.w; do
            ((total=total+1))

            echo $file

            output_file=${file##*/}
            output_file=${output_file%.w}

            rm -f bin/$output_file
            if "./bin/wlang${EXECUTABLE_EXTENSION}" -q -O bin -o $output_file $file; then
                output=$(./bin/$output_file)
                echo "$output"

                if [[ "$output" != *"Incorrect"* ]] && [[ "$output" == *"Ok"* ]]; then
                    ((passed=passed+1))
                else
                    failures+=($file)
                fi
            else
                failures+=($file)
            fi
        done

        if [[ $total -eq $passed ]]; then
            echo $'\u001b[1m\u001b[32m'$passed/$total tests passed!$'\u001b[0m'
        else
            echo $'\u001b[1m\u001b[31m'$passed/$total tests passed!$'\u001b[0m'
            for failure in ${failures[@]}; do
                echo "    "$'\u001b[31m'$failure failed.$'\u001b[0m'
            done
        fi
    fi
fi
