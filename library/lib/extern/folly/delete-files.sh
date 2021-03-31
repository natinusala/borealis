#!/bin/bash

cd folly

if (($?>0)); then
    echo "Can't find working directory... exiting"
    exit
fi

for i in *; do
    if ! grep -qxFe "$i" filelist.txt; then
        echo "Deleting: $i (but not really)"
        rm -rf "$i"
    fi
done