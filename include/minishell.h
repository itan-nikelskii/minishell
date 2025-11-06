/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 09:07:42 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 14:50:19 by antoniocoss      ###   ########.fr       */
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
# include "../libft/include/libft.h"
# include "parser.h"

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

/* Note: Main structures (t_token_type, t_token_t, t_redir, t_command, */
/* t_shell, t_parse_result) are now defined in parser.h */

/* Multi-pipe execution context */
typedef struct s_pipe_ctx
{
	int			cmd_count;
	int			(*pipes)[2];
	pid_t		*pids;
	t_shell		*shell;
}	t_pipe_ctx;

/* ************************************************************************** */
/*                              EXECUTOR                                      */
/* ************************************************************************** */

/* Main executor */
int		execute_command(t_command *cmd, t_shell *shell);

/* Single command executor */
int		execute_single_command(t_command *cmd, t_shell *shell);

/* Pipeline executor (handles 2+ commands) */
int		execute_pipeline(t_command *cmd_list, t_shell *shell);

/* Pipeline utils */
void	cleanup_pipes(int pipefd[2]);
void	close_all_pipes(t_pipe_ctx *px, int count);
int		create_all_pipes(t_pipe_ctx *px);
int		wait_all_children(pid_t *pids, int n);

/* External command executor (in child process) */
int		exec_external_in_child(t_command *cmd, t_shell *shell);

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
bool	too_many_heredocs(t_command *cmd);

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

/* Redirection utils */
void	cleanup_redir_fds(int in_fd, int out_fd);

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
void	cleanup_and_exit(t_shell *shell, int status);

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

/* Note: Parser functions (parse, free_commands) are declared in parser.h */

/* ************************************************************************** */
/*                          CONTINUATION (PS2)                                */
/* ************************************************************************** */

char	*process_continuation(char *line, t_shell *shell);

/* ************************************************************************** */
/*                              GLOBAL VARIABLES                              */
/* ************************************************************************** */

extern volatile sig_atomic_t	g_signal_received;

#endif
