import gdb

def getCurrentFileSymbols():
    functions = gdb.execute("info functions -q -n", to_string=True)
    functions = functions.split("\n")
    filename = gdb.lookup_symbol("main")[0].symtab.filename
    # [:] is needed because we should NEVER delete itens from iterated list
    for item in functions[:]:
        if filename not in item:
            functions.remove(item)
        else:
            functions.remove(item)
            for item in functions[:]:
                if "File " not in item:
                    curr_file_functions.append(item)
                else:
                    break

    print(curr_file_functions)

def checkFrameName():
    for item in curr_file_functions:
        if gdb.selected_frame().name():
            if gdb.selected_frame().name() in item:
                return True

    return False

def set_logfile(file, ovewrite):
    gdb.execute("set logging enabled off")
    if ovewrite == False:
        gdb.execute("set logging overwrite off")
    else:
        gdb.execute("set logging overwrite on")
    gdb.execute("set logging file /home/brunosmp/Documents/UNIFESP/TCC2/development/tmp/"+file)
    gdb.execute("set logging enabled on")

# just set the stack pointer BP
# first time we change to main and $sp changes
# save it, this is __libc_start_call_main return rip
class spBreakpoint(gdb.Breakpoint):
    def __init__(self):
        gdb.Breakpoint.__init__(self, "$sp", type=gdb.BP_WATCHPOINT)

        # get current sp and cast it to address
        self.old_sp = gdb.parse_and_eval("$rsp")

        # this is the same as old for now
        self.curr_sp = self.old_sp

    def stop(self):
        if gdb.selected_frame().name() == "main":
            # first set logfile
            set_logfile("gdb.tmp2", False)

            # get the difference to print
            self.sp_diff = self.curr_sp - self.old_sp

            # print the difference in hex byte to byte
            gdb.execute("echo Begin_stack_change:\\n")

            # from SP up, print sp_diff bytes (remember that sp growns down, so it will change for instance
            #   from 20 to 10, so we'll print 16 bytes starting from 10, which will go up to 20
            gdb.execute("x/"+str(abs(self.sp_diff))+"xb $sp")

            # this above works but TODO find out how to say this to user
            gdb.execute("echo End_stack_change:\\n")

            # reset logfile
            set_logfile("gdb.tmp", False)

            self.libc_rip = gdb.parse_and_eval("$sp")
            # get sp address and cast it from (void *) to a 64bit integer
            self.libc_rip_sp = self.libc_rip.cast(gdb.lookup_type("uint64_t").pointer())
            # dereference sp and get effective libc address and cast it
            self.libc_rip = self.libc_rip_sp.dereference().cast(gdb.lookup_type("uint64_t").pointer())
            gdb.set_convenience_variable("libc_rip_sp", self.libc_rip_sp)
            gdb.set_convenience_variable("libc_rip", self.libc_rip)
            gdb.execute("disable "+str(self.number))

        else:
            # save curr_sp
            self.old_sp = self.curr_sp
            # get new sp
            self.curr_sp = gdb.parse_and_eval("$rsp")

        return False

# when we hit __libc_start_call_main
class libcBreakpoint(gdb.Breakpoint):
    def __init__(self):
        gdb.Breakpoint.__init__(self, "__libc_start_call_main", temporary=True)

    def stop(self):
        self.stackBP = spBreakpoint()
        return False

class watchSP(gdb.Breakpoint):
    def __init__(self):
        gdb.Breakpoint.__init__(self, "$sp", type=gdb.BP_WATCHPOINT)
        # get current sp and cast it to address
        self.old_sp = gdb.parse_and_eval("$rsp")
        #self.old_sp = self.old_sp.cast(gdb.lookup_type("uint64_t").pointer())
        # this is the same as old for now
        self.curr_sp = self.old_sp

    def stop(self):
        # everytime sp changes

        # we only save the frame change if our current frame
        # is in our main file
        if checkFrameName():
            # first set logfile
            set_logfile("gdb.tmp2", False)

            # save curr_sp
            self.old_sp = self.curr_sp
            # get new sp
            self.curr_sp = gdb.parse_and_eval("$rsp")
            # get the difference to print
            self.sp_diff = self.curr_sp - self.old_sp

            # print the difference in hex byte to byte
            gdb.execute("echo Begin_stack_change:\\n")

            # from SP up, print sp_diff bytes (remember that sp growns down, so it will change for instance
            #   from 20 to 10, so we'll print 16 bytes starting from 10, which will go up to 20
            gdb.execute("x/"+str(abs(self.sp_diff))+"xb $sp")

            # this above works but TODO find out how to say this to user
            gdb.execute("echo End_stack_change:\\n")
            gdb.execute("bt")

            # reset logfile
            set_logfile("gdb.tmp", False)

        return False

class tutorialMainBP(gdb.Breakpoint):
    def __init__(self):
        # When we break on main:
        # Maybe it's better to run a function?
        # 1) info frame
        #   1.1) save $rip;
        #   1.2) save $prev_rip
        #       1.1.1) check how to transfer convenience vars to python vars?
        #   1.2) show that $prev_rip from main is glibc calling function
        #   1.3)
        gdb.Breakpoint.__init__(self, "main", temporary=True)

    def stop(self):
        set_logfile("gdb.tmp2", False)
        #gdb.execute("set trace-commands on")

        self.libc_rip = gdb.parse_and_eval("$libc_rip")
        gdb.execute("echo Start command x/8xg $sp\\n")
        print(self.libc_rip)
        gdb.execute("echo End command x/8xg $sp\\n")
        gdb.execute("set listsize 1")
        gdb.execute("echo Start command info lines *($sp)\\n")
        gdb.execute("list *$libc_rip-1,")
        gdb.execute("echo End command info lines *($sp)\\n")
        #print(self.libc_rip.format_string(symbols=True, address=True))
        gdb.execute("set listsize 10")
        gdb.execute("echo Start command list main,\\n")
        gdb.execute("list main,")
        gdb.execute("echo End command list main,\\n")

        #gdb.execute("set trace-commands off")
        set_logfile("gdb.tmp", False)

        self.wsp = watchSP()

        return False

curr_file_functions = []

getCurrentFileSymbols()

libcBreakpoint()
mainbp = tutorialMainBP()
