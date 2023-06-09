set logging file ./tmp/gdb.tmp
set logging overwrite on
set logging redirect on
set logging debugredirect on
set logging enabled on

set debuginfod enabled on
set trace-commands on

set follow-fork-mode parent
set detach-on-fork off

source ./gdb_utils/exec_gdb.py

info functions
# this is hard_coded and should be immediately before waitpid,
# so fork is already called but we aren't waiting yet
# this way it's possible to detach from child without execution stopping
# TODO:(?) maybe I could get it parsing shell.c file, i don't know
break shell.c:152

break execvp

catch exec

#note to self:
# command X will execute until the end before passing control, so
# after running "inferior X", it'll first run the next commands
# and THEN pass control to "inferior X"
# that's why "detach" and "remove" are on command 2 and not
# command 1, control will NEVER RETURN TO COMMAND 1, only
# when breakpoint is hit again

command 1
	info inferiors
	#pi is an alias to "python-intepreter" and it runs the python code that follows
	pi gdb.execute("inferior "+str(gdb.inferiors()[1].num))
	continue
end

command 2
	finish
	continue
end

command 3
	source ./gdb_utils/teste_bp.py
	list 1,
	#info variables -n -q
	#pi gdb.execute("inferior "+str(gdb.inferiors()[0].num))
	#pi gdb.execute("detach inferior "+str(gdb.inferiors()[1].num))
	#pi gdb.execute("remove-inferiors "+str(gdb.inferiors()[1].num))
	continue
end

run
