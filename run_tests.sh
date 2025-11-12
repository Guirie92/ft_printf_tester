#!/bin/bash

rm -rf *.o tester

cd ..

make -s fclean

make -s

cd -

cc -w ft_printf_tester.c \
    -L ..                \
    -lftprintf           \
    -o tester

./tester