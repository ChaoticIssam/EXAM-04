#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int	put_err(char *s1, char *s2){
	int	i = 0;
	while (s1[i])
		i++;
	write (2, s1, i);
	i = 0;
	while (s2[i])
		i++;
	write (2, s2, i);
	write (2, "\n", 1);
	return 1;
}

int	exec(char **av, char **env, int i){
	av[i] = NULL;
	execve(av[0], av, env);
	put_err("error: cannot execute ", av[0]);
	return 1;
}

int	main(int ac, char **av, char **env){
	int	i = 0;
	int fd0 = dup(0);
	int	fd[2];
	(void)ac;

	while (av[i] && av[i + 1]){
		av = &av[i+1];
		i = 0;
		printf("1\n");
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		printf("2\n");
		if (i != 0 && strcmp(av[0], "cd") == 0){
			if (i != 2)
				put_err("error: dyal largs", "");
			else if (chdir(av[1]) != 0)
				put_err("error: dyal dir", av[1]);
			printf("3\n");
		}
		else if (i != 0 && (av)){
			int	pid = fork();
			if (pid ==0){
				exec(av, env, i);
				return 1;
			}
			else{
				dup2(fd0, 0);
				waitpid(pid, NULL, 2);
			}
			printf("4\n");
		}
		else if (i != 0 && strcmp(av[i], "|") == 0){
			pipe(fd);
			if (fork() == 0){
				dup2(fd[1], 1);
				close(fd[1]);
				close(fd[0]);
				close(fd0);
				exec(av, env, i);
				return 1;
			}
			else{
				dup2(fd[0], 0);
				close(fd[1]);
				close(fd[0]);
			}
		}
	}
	close (fd0);
	return 0;
}