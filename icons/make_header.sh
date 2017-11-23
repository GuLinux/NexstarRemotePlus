#!/bin/bash
rm tmp_*
rm icons.h
cat >icons.h <<EOF
#pragma once
EOF

for file in *.png; do
    name="$( basename $file .png)"
    convert $file -flatten -negate -resize 16x16 -compress none tmp_$name.pgm
    sep=''
    index=0
    echo "const PROGMEM uint8_t icon_$name[] = {" >> icons.h
    for value in $(tail -n +4 tmp_bluetooth.pgm); do
        ((index++))
        [[ "$value" -gt 128 ]] && value_treshold=1 || value_treshold=0
        echo -n "${sep}${value_treshold}" >> icons.h
        sep=', '
        [[ "$(( $index % 16 ))" == "0" ]] && echo >> icons.h
    done
    echo "};" >> icons.h
done

