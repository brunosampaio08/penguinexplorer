set debuginfod enabled on

break _start

command 1
	source ./tutorial.2.py
	disable $_hit_bpnum
	continue
end

run
