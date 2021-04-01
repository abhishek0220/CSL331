build:
	gcc 087-Task1.c -o 087-Task1
	gcc 087-Task2.c -o 087-Task2
	gcc 087-Task3.c -o 087-Task3

clean:
	rm -f num.txt
	rm -f 087-Task1
	rm -f 087-Task2
	rm -f 087-Task3
	rm -f 087-Task2-Child.c
	rm -f 087-Task2-Child