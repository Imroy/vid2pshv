pshv_encoder: main.c
	$(CC) $(CFLAGS) $< -o $@

install: pshv_encoder
	sudo install -s $< /usr/local/bin/