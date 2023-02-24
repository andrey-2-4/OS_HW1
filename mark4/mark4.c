#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	// было 4096
	int size = 5000;
	// ./a.out input output
	// input - argv[1] - файл для открытия
	// output - argv[2] - файл для записи
	
  	// чтение
	int fd;
	char buffer[size];
	ssize_t read_bytes;
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		printf("Can\'t open file\n");
		exit(-1);
	}
	read_bytes = read(fd, buffer, size);
	if (read_bytes == -1) {
		printf("Can\'t read this file\n");
		exit(-1);
	}
	buffer[read_bytes] = '\0';
	if (close(fd) < 0) {
		printf("Can\'t close file\n");
	}
	
	// создаем второй процесс, который будет обрабатывать файл
	// если это родитель (процесс 1), то мы передаем через НЕИМЕНОВАННЫЙ канал данные ребенку (процессу 2)
	// иначе наоборот получаем
	int    fd12[2], result;
	char   str_buf[read_bytes];
	
	if(pipe(fd12) < 0) {
    		printf("Can\'t open pipe\n");
    		exit(-1);
	}

	result = fork();
	if(result < 0) {
		printf("Can\'t fork child\n");
		exit(-1);
	} else if (result > 0) { /* процесс 1 пишет в канал */
		if(close(fd12[0]) < 0){
			printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
		}
		size = write(fd12[1], buffer, read_bytes);
		if(size != read_bytes){
			printf("Can\'t write all string to pipe\n");
			 exit(-1);
		}
		if(close(fd12[1]) < 0) {
			printf("parent: Can\'t close writing side of pipe\n");
			exit(-1);
		}
		// printf("Parent exit\n");
	} else { /* процесс 2 читает из канала, обрабатывает, передает процессу 3 */
		if(close(fd12[1]) < 0){
			printf("child: Can\'t close writing side of pipe\n"); exit(-1);
		}
		size = read(fd12[0], str_buf, read_bytes);
		if(size < 0){
  			printf("Can\'t read string from pipe\n");
			exit(-1);
		}
		// str_buf - это то, что прочитал 2 процесс из канала
		// printf("Child exit, str_buf: %s\n", str_buf);
		if(close(fd12[0]) < 0){
			printf("child: Can\'t close reading side of pipe\n"); exit(-1);
		}
		
		// здесь должна быть обработка
		
		// создаем процесс 3
		if(pipe(fd12) < 0) {
	    		printf("Can\'t open pipe\n");
	    		exit(-1);
		}
		result = fork();
		if(result < 0) {
			printf("Can\'t fork child\n");
			exit(-1);
		} else if (result > 0) { /* процесс 2 пишет в канал */
			if(close(fd12[0]) < 0){
				printf("parent: Can\'t close reading side of pipe\n"); exit(-1);
			}
			size = write(fd12[1], buffer, read_bytes);
			if(size != read_bytes){
				printf("Can\'t write all string to pipe\n");
				 exit(-1);
			}
			if(close(fd12[1]) < 0) {
				printf("parent: Can\'t close writing side of pipe\n");
				exit(-1);
			}
			// printf("Parent exit\n");
		} else { /* процесс 3 читает из канала, затем пишет в файл*/
			if(close(fd12[1]) < 0){
				printf("child: Can\'t close writing side of pipe\n"); exit(-1);
			}
			size = read(fd12[0], str_buf, read_bytes);
			if(size < 0){
	  			printf("Can\'t read string from pipe\n");
				exit(-1);
			}
			// str_buf - это то, что прочитал 3 процесс из канала
			// printf("Child exit, str_buf: %s\n", str_buf);
			if(close(fd12[0]) < 0){
				printf("child: Can\'t close reading side of pipe\n"); exit(-1);
			}
			// запись
			int len = read_bytes - 1;
			if ((fd = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
				printf("Can\'t open file\n");
				exit(-1);
			}
			size = write(fd, str_buf, len);
			if (size != len) {
				printf("Can\'t write all string\n");
				exit(-1);
			}
			if (close(fd) < 0) {
				printf("Can\'t close file\n");
			}
		}
	}
	
	
	return 0;
}
