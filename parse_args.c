//fxnbr: 5
#include "philo.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int is_numeric(const char *str)// guarda sotto
{
    if (*str == '\0')
        return 0;
    while (*str)
    {
        if (*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

int	ft_atoi(const char *str)
{
	int					i;
	int					sign;
	unsigned long int	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == 43)
		i++;
	else if (str[i] == 45)
	{
		sign *= -1;
		i++;
	}
	while (ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - 48;
		i++;
	}
	result *= sign;
	return (result);
}

void validate_argouments(int argc, char **argv)
{
	int i;

    if (argc != 5 && argc != 6)
	{
        ft_printf("Usage: <program_name> <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [number_of_times_each_philosopher_must_eat]\n");
        exit(EXIT_FAILURE);
        // FIXME: guarda
    }
    i = 1;
    while (i < argc)
	{
        if (!is_numeric(argv[i])) // puoi usare benissimo isdigit e risparmiarti una funzione
        {
            ft_printf("Error: Argument %d should be a number\n", i);
            exit(EXIT_FAILURE);
        }
        i++;
    }
}

int parse_args(int argc, char **argv, t_argouments *args)
{
    validate_argouments(argc, argv);
    args->philo_nbr = ft_atoi(argv[1]);
    args->time_to_die = ft_atoi(argv[2]);
    args->time_to_eat = ft_atoi(argv[3]);
    args->time_to_sleep = ft_atoi(argv[4]);
    if (argc == 6)
        args->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
	else
        args->number_of_times_each_philosopher_must_eat = -1;
    if (args->philo_nbr < 0 || args->philo_nbr > 200 ||
        args->time_to_die < 60 || args->time_to_die > 28800000 || 
        args->time_to_eat < 60 || args->time_to_eat > 28800000 || 
        args->time_to_sleep < 60 || args->time_to_sleep > 28800000 || 
        (argc == 6 && (args->number_of_times_each_philosopher_must_eat < 0 || 
                       args->number_of_times_each_philosopher_must_eat > 28800000)))
        return (FAILURE);
    else 
        return (SUCCESS);
}