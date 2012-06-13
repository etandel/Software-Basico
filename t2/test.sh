#!/bin/bash

base=.
if [ -x $base/test ]; then
    find $base/tests -type f \
        -exec echo -e "\nTestando {}" \; \
        -exec $base/test {} \;
else
    echo "Erro: Executavel $base/test nao existe."
fi
