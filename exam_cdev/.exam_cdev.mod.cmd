savedcmd_exam_cdev.mod := printf '%s\n'   exam_cdev.o | awk '!x[$$0]++ { print("./"$$0) }' > exam_cdev.mod
