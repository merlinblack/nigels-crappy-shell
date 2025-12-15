.PHONY: all
all: ncs token piper

ncs: main.c str_delimit.c
	gcc -g --std=gnu23 -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 main.c str_delimit.c -lreadline -o ncs

token: str_delimit.c
	gcc -g --std=gnu23 -DTEST_DELIMIT str_delimit.c -o token

piper: piper.c
	gcc -g --std=gnu23 piper.c -o piper

clean:
	rm ncs token piper
