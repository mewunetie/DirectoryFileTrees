ft: ft_client.o ft.o node.o dynarray.o
	gcc217 -g ft_client.o ft.o node.o dynarray.o -o ft

ft.o: ft.h ft.c node.h dynarray.h a4def.h
	gcc217 -g -c ft.h ft.c node.h dynarray.h a4def.h

node.o: node.h node.c dynarray.h a4def.h
	gcc217 -g -c node.h node.c dynarray.h a4def.h

dynarray.o: dynarray.h dynarray.c
	gcc217 -g -c dynarray.h dynarray.c

ft_client.o: ft_client.c ft.h a4def.h
	gcc217 -g -c ft_client.c ft.h a4def.h
