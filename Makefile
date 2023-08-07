build:
	tcc -Wfatal-errors \
	-std=c17  \
	./main.c \
	-s -Ofast \
	-o app.exe 