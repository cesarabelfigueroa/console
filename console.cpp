#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main() { 
	int status;
	char linea[20];
	char content[20];
	char *command;
	char *param;
	char *oper;
	char *param1;
	char *params;
	int pid;
	FILE *fd,*fd1;
	char c;

	while (1) {
		command = NULL;
		param = NULL;		
		cout << ":>>";
		cin.getline(linea, 20);
		command = strtok(linea," ");
		param = strtok(NULL," ");
		oper = strtok(NULL," ");
		if (oper != NULL) {
			params = oper;
			oper = param;
			param = params;
			if (strcmp(oper,">>") == 0) {
				if(strcmp(command, "cat") == 0){
					param1 = strtok(NULL," ");
					fd = fopen(param,"r");
					fd1 = fopen(param1,"a");
					int i=0;
		  			while (1) {
		    			c = (char)fgetc(fd);
						if (c != EOF) {
							fputc(c,fd1);
						} else 
						break;
					}
					fclose(fd);
					fclose(fd1);
				}else{
					cout << "command not found." << endl;
				}	
			}else if(strcmp(oper,">") == 0){
				param1 = strtok(NULL," ");
				fd = fopen(param,"w");
	  			while (1) {
	    			cin.getline(content, 20);
					if(strcmp(content,":q") == 0){
						break;
					}else{
						strcat(content, "\n");
						fputs (content,fd);
					}
				}
				fclose(fd);
			}	   		
		} else if ((pid = fork()) == 0) { 
			if(execlp(command, command, param, NULL)){
				cout <<  "command not found." << endl;
			}
		} 
	}
	cout << "Esperando ..." << endl;
	waitpid(pid, &status,0);
	cout << "estado: " << status << endl;
	return 0;
}