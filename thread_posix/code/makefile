CFLAGS+=-pthread
LDFLAGS+=-pthread


mytbf:main.o mytbf.o
	gcc $^ -o $@

clean:
	rm -rf *.o mytbf