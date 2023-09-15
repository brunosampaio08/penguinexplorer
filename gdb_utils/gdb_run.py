import gdb

CMD_FCT  = "gdb_run"
CMD_NAME = "str"

class GdbRunBreakpoint(gdb.Breakpoint):
    def __init__(self):
        gdb.Breakpoint.__init__(self, CMD_FCT, internal=1)
        self.silent = True

    def stop(self):
        cmd = gdb.parse_and_eval(CMD_NAME).string()
        gdb.execute(cmd)
        return False
GdbRunBreakpoint()
