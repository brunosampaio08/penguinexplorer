# syntax=docker/dockerfile:1
FROM archlinux:base-devel
WORKDIR /docker
COPY . .
RUN pacman -Syyu --noconfirm ncurses gdb
RUN make
ENTRYPOINT "/docker/bin/pE_application"
