#ifndef PHILO_H
# define PHILO_H

#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define BLACK   "\033[0;30m"
#define RED     "\033[1;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define ORANGE  "\033[38;5;214m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[0;37m"
#define RESET   "\033[0m"

#define SUCCESS 0
#define FAILURE -1

typedef struct s_fork t_fork;
typedef struct s_table t_table;
typedef struct s_queue t_queue;
typedef struct s_print t_print;
typedef struct s_supervisor t_supervisor;

typedef struct s_argouments
{
	int	philo_nbr;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	number_of_times_each_philosopher_must_eat;
}	t_argouments;

typedef struct s_philo 		
{
	int			id;
	int			number_of_meals_eaten;
	long		last_meal_time;   	//QUANTO TEMPO E' PASSATO DALL'ULTIMO PASTO?
	bool		full_stomach; 	  	// 1 = sazio
    bool        rogue;              // 0 = honest; 1 = rogue
	t_fork		*left_fork;
	t_fork		*right_fork;
	t_table		*table;
	pthread_t	thread; 		// feed into pthread_create
	pthread_t	supervisor_thread; // controllo sazietà/morte
}	t_philo;

typedef struct s_fork
{
	pthread_mutex_t	fork;
	int				fork_id;
}	t_fork;

typedef struct s_print
{
    bool end;
	pthread_mutex_t lock_print; // serve a qualcosa?
	pthread_mutex_t lock_check; // serve a qualcosa?
    pthread_mutex_t print_mutex;
    pthread_mutex_t kill_mutex;
} t_print;

typedef struct s_message
{
    int             id;
    long            timestamp; // NOTE questo deve essere il dato del filosofo
    char            *string;
    struct s_message *next;
} t_message;

typedef struct s_queue
{
	pthread_t       thread;
    t_message       *head;
    t_message       *tail;
    pthread_mutex_t mutex;
    bool            end;
} t_queue;

typedef struct s_supervisor
{
	pthread_mutex_t lock_last_meal_time;
	pthread_mutex_t lock_number_meals_eaten;
	pthread_mutex_t lock_full_stomach;
	pthread_mutex_t lock_end;            // table->end
} t_supervisor;

typedef struct s_table
{
	t_argouments	args;
	t_fork			*fork;		// array of forks e quindi plurale!
	t_philo			*philos;
	t_queue			queue;
	t_print			print;
	t_supervisor	supervisor;      
	long			start_time;
	bool			end; 	// one dies or all have full_stomach
}	t_table;


// main.c
void *routine(void *arg);
void *supervisor(void *arg);
int start_simulation(t_table *table);
void end_simulation(t_table *table);

// utils.c
int ft_atoi(const char *str);
int ft_isdigit(int c);
void ft_bzero(void *s, size_t n);
void *ft_memcpy(void *dst, void *src, size_t n);
void *safe_alloc(size_t count, size_t size, const char *err_msg);
size_t ft_strlen(const char *s);
char *ft_strcpy(char *dst, const char *src);
char *ft_strdup(const char *src);

// print.c
int ft_printf(const char *str, ...);
void ft_putchar(char ch, int *c);
void ft_putnbr(long n, int *c);
void ft_putstr(char *str, int *c);
void ft_vcheck(const char v, va_list ptr, int *c);
int print_event(t_philo *philo, const char *str);
void *queue(void *arg);
void set_print_end(t_table *table);
int check_end(t_philo *philo);

// parse_args.c
void validate_argouments(int argc, char **argv);
int parse_args(int argc, char **argv, t_argouments *args);

// init_simulation.c
int init_philos(t_table *table);
int init_print(t_table *table);
int init_supervisor_mutexes(t_table *table);
int init_supervisor(t_table *table);
void init_message_queue(t_queue *q);
void init_table(t_table *table);
void wait_for_threads(t_table *table);

// events.c
int eat(t_philo *philo);
int philo_sleep(t_philo *philo);
void think(t_philo *philo);
void philo_take_forks(t_philo *philo);
void philo_release_forks(t_philo *philo);
void safe_set(pthread_mutex_t *mutex, void *src, void *dst, size_t size);
void safe_get(pthread_mutex_t *mutex, void *src, void *dst, size_t size);
void set_end_time(t_table *table, bool value);
int get_end_time(t_table *table);
bool check_death(t_philo *philo, long now, long last_meal);
int add_print_event(long ms, t_philo *philo, const char *str);
void stop_print(t_table *table);
int launch_print(t_table *table);
int lock_unlock_forks(t_philo *philo, int flag);
int lock_forks(t_philo *philo);
int unlock_forks(t_philo *philo);
int handle_single_philo(t_philo *philo);
void pretty_output(long time_ms, int max_width, int philosopher_id, const char *event);
unsigned long get_time(void);
void cleanup(t_table *table);

#endif
