set logging file ./tmp/gdb.tmp
set logging overwrite on
set logging redirect on
set logging debugredirect on
set logging enabled on

set follow-fork-mode parent

info functions

break run_shell
	command 1
	p unparsed_cmd
	continue
end

run
