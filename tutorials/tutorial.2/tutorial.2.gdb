set debuginfod enabled on
set debuginfod urls https://debuginfod.archlinux.org
set debuginfod verbose 0

break _start

command 1
	source ./tutorial.2.py
	disable $_hit_bpnum
	continue
end

run
