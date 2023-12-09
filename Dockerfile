# syntax=docker/dockerfile:1
FROM archlinux:base-devel
WORKDIR /docker
COPY . .
#COPY intro Makefile pE_application.c pE_application.h shell.c shell.h tutorials ./
RUN pacman -Sy --noconfirm ncurses gdb
RUN make --debug=basic
ENTRYPOINT "/docker/bin/pE_application"
