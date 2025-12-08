ncs: main.c
	gcc --std=gnu23 -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 main.c -lreadline -o ncs

clean:
	rm ncs
