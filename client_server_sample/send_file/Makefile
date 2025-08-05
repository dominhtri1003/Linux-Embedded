.PHONY:clean all

FLAGS:=-pthread -g
all :server.c client.c
	gcc $(FLAGS) server.c -o server 
	gcc $(FLAGS) client.c -o client 

*.o:*.c
	gcc -c  $< -o $@
clean:
	rm -f *.o