

.PHONY: all

all:read_config read_config_cursed

read_config:read_config.c
	cc -Wall -Wpedantic read_config.c -o read_config

read_config_cursed:read_config_cursed.c
	cc -Wall -Wpedantic read_config_cursed.c -o read_config_cursed
