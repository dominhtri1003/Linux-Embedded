savedcmd_pass_arg.mod := printf '%s\n'   pass_arg.o | awk '!x[$$0]++ { print("./"$$0) }' > pass_arg.mod
