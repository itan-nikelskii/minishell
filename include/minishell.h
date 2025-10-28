/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 09:07:42 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 20:01:56 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* Feature test macro for POSIX.1-2008 (needed for struct sigaction) */
# define _POSIX_C_SOURCE 200809L

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../../libft/include/libft.h"

/* ************************************************************************** */
/*                              CONSTANTS                                     */
/* ************************************************************************** */

/* Exit codes */
# define CMD_NOT_EXECUTABLE 126
# define CMD_NOT_FOUND 127

/* Buffer sizes */
# define HD_PATH_BUFSZ 256

/* ************************************************************************** */
/*                              STRUCTURES                                    */
/* ************************************************************************** */

/* Type of token */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
}	t_token_type;

/* Token structure */
typedef struct s_token
{
	t_token_type	type;
	char			*text;
	struct s_token	*next;
}	t_token_t;

/* Redirection entry */
typedef struct s_redir
{
	t_token_type	type;
	char			*target;
	bool			was_quoted;
	char			*hd_path;
	struct s_redir	*next;
}	t_redir;

/* Command node (in a pipeline) */
typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_command;

/* Shell state - global state of the shell */
typedef struct s_shell
{
	char	**envp;
	char	**xenv;
	int		last_exit_status;
	int		stdin_backup;
	int		stdout_backup;
	bool	interactive;
	bool	in_child;
}	t_shell;

/* Multi-pipe execution context */
typedef struct s_pipe_ctx
{
	int			cmd_count;
	int			(*pipes)[2];
	pid_t		*pids;
	t_shell		*shell;
}	t_pipe_ctx;

/* Parser result structure */
typedef struct s_parse_result
{
	t_command	*commands;
	char		*error;
	bool		incomplete_pipe;
}	t_parse_result;

/* ************************************************************************** */
/*                              EXECUTOR                                      */
/* ************************************************************************** */

/* Main executor */
int		execute_command(t_command *cmd, t_shell *shell);

/* Single command executor */
int		execute_single_command(t_command *cmd, t_shell *shell);

/* Pipeline executors */
int		execute_pipeline(t_command *cmd_list, t_shell *shell);
int		execute_pipeline_multi(t_command *cmd_list, t_shell *shell);

/* Pipeline utils */
void	close_all_pipes(t_pipe_ctx *px, int count);
int		create_all_pipes(t_pipe_ctx *px);
int		wait_all_children(t_pipe_ctx *px);

/* External command executor */
int		execute_external(t_command *cmd, t_shell *shell);

/* ************************************************************************** */
/*                              BUILTINS                                      */
/* ************************************************************************** */

int		is_builtin(char *cmd);
int		execute_builtin(t_command *cmd, t_shell *shell);

int		builtin_echo(t_command *cmd, t_shell *shell);
int		builtin_cd(t_command *cmd, t_shell *shell);
int		builtin_pwd(t_command *cmd, t_shell *shell);
int		builtin_export(t_command *cmd, t_shell *shell);
int		builtin_unset(t_command *cmd, t_shell *shell);
int		builtin_env(t_command *cmd, t_shell *shell);
int		builtin_exit(t_command *cmd, t_shell *shell);

/* ************************************************************************** */
/*                              REDIRECTIONS                                  */
/* ************************************************************************** */

int		setup_redirections(t_redir *redirs, int *in_fd, int *out_fd);
int		apply_redirections(int in_fd, int out_fd);
int		save_std_fds(t_shell *shell);
int		restore_std_fds(t_shell *shell);

/* ************************************************************************** */
/*                              HEREDOC                                       */
/* ************************************************************************** */

/* Heredoc preprocessing */
int		prepare_heredocs(t_command *cmd, t_shell *shell);
int		get_heredoc_counter(void);
void	build_heredoc_filepath(char *filepath);
char	*hd_expand_line(const char *line, t_shell *shell, bool expand);
int		hd_append_str(char **buf, const char *str);
int		hd_append_char(char **buf, char c);
char	*hd_getenv(t_shell *shell, char *name);
char	*hd_extract_varname(const char *str, size_t i);
char	*read_heredoc_line(t_shell *shell);

/* ************************************************************************** */
/*                              UTILS                                         */
/* ************************************************************************** */

/* Path resolution */
char	*resolve_path(char *command, char **envp);
char	*get_env_value(char *key, char **envp);

/* Environment utils */
char	**env_dup(char **envp);
int		env_update(t_shell *shell, char *key, char *value);
int		env_remove(t_shell *shell, char *key);

/* Export utils */
void	print_export_line(char *env_var);
bool	is_valid_export_identifier(char *str);

/* export: xenv and print-merge */
int		xenv_add(t_shell *sh, const char *key);
int		xenv_remove(t_shell *sh, const char *key);
int		print_export_merged(t_shell *sh);

/* Error handling */
void	print_error(char *prefix, char *msg);
void	print_error_arg(char *prefix, char *arg, char *msg);
void	print_perror(char *prefix, char *arg);
void	cmd_error_exit(const char *cmd, const char *msg, int code);
void	file_error_exit(const char *filename, int fd);

/* Process utils */
int		map_execve_errno(void);
int		get_child_exit_status(int status);

/* Execve utils */
void	execve_or_die(t_command *cmd, t_shell *shell);
void	exec_child_piped(t_command *cmd, t_shell *shell);
void	exec_child_single(t_command *cmd, t_shell *shell);

/* Command utils */
int		count_commands(t_command *cmd_list);
int		is_last_command(t_command *cmd);

/* Array utils */
int		count_array(char **array);
int		keycmp(const char *a, const char *b);
void	sort_env_array(char **env);
void	array_copy_except(char **dest, char **src, int skip_idx, int total);

/* Search utils */
int		find_in_array(char **arr, const char *key, bool require_equal);
int		find_env(char **envp, char *key);
int		find_xenv(char **xenv, const char *key);

/* Exit utils */
int		is_valid_exit_arg(const char *str, long long *out);

/* Memory utils */
void	free_array(char **array);
void	free_cmd_list(t_command *cmd_list);
void	free_redir_list(t_redir *redir_list);

/* Shell lifecycle */
t_shell	*shell_init(char **envp);
int		shell_cleanup(t_shell *shell);

/* Input utils */
char	*read_input_line(t_shell *shell);

/* ************************************************************************** */
/*                              SIGNALS                                       */
/* ************************************************************************** */

void	setup_parent_ps1_signals(void);
void	setup_parent_wait_signals(void);
void	setup_exec_signals(void);
void	setup_child_ps2_signals(void);
void	reset_signals(void);

/* ************************************************************************** */
/*                              PARSER                                        */
/* ************************************************************************** */

int		parse(const char *line, t_parse_result *result, t_shell *shell);
void	free_commands(t_command *cmd);

/* ************************************************************************** */
/*                          CONTINUATION (PS2)                                */
/* ************************************************************************** */

char	*process_continuation(char *line, t_shell *shell);

/* ************************************************************************** */
/*                              GLOBAL VARIABLES                              */
/* ************************************************************************** */

extern volatile sig_atomic_t	g_signal_received;

#endif
