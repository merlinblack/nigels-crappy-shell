.PHONY: all
all: ncs token

ncs: main.c str_delimit.c
	gcc -g --std=gnu23 -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 main.c str_delimit.c -lreadline -o ncs

token: str_delimit.c
	gcc -g --std=gnu23 -DTEST_DELIMIT str_delimit.c -o token

clean:
	rm ncs token
