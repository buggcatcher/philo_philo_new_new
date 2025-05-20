//fxnbr: 2
#include "philo.h"

int main(int argc, char **argv)
{
    t_table table;

    if (parse_args(argc, argv, &table.args) == FAILURE)
    {
        printf("Errore: parametri non validi.\n");
        return (1);
    }
    if (start_simulation(&table) == FAILURE)
    {
        printf("Errore: inizializzazione fallita.\n");
        cleanup(&table);
        return (1);
    }
    end_simulation(&table);
    return (0);
}

// pthread create richiede un puntatore a void
void *routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	if (philo->id % 2 == 0)
		usleep(100);
	while (42)
	{
		if (eat(philo) == FAILURE)
			return (NULL);
		if (check_end(philo) == FAILURE)
			return (NULL);
		if (philo_sleep(philo) == FAILURE)
			return (NULL);
		if (print_event(philo, "is thinking") == FAILURE)
            return (NULL);
		if (check_end(philo) == FAILURE)
			return (NULL);
	}
	return (NULL);
}

void *supervisor(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    long now;
    long last_meal;
    int meals;
    bool end = false;

    while (42)
    {
        safe_get(&philo->table->supervisor.lock_end, &philo->table->end, &end, sizeof(bool));
        if (end)
            return (NULL);
        safe_get(&philo->table->supervisor.lock_last_meal_time, &philo->last_meal_time, &last_meal, sizeof(long));
        now = get_time();
        if (check_death(philo, now, last_meal))
            return (NULL);
        safe_get(&philo->table->supervisor.lock_number_meals_eaten, &philo->number_of_meals_eaten, &meals, sizeof(int));
        if (philo->table->args.number_of_times_each_philosopher_must_eat > 0 && meals >= philo->table->args.number_of_times_each_philosopher_must_eat)
            safe_set(&philo->table->supervisor.lock_full_stomach, &(bool){true}, &philo->full_stomach, sizeof(bool));
        usleep(1000); // riduce carico CPU, ma tiene monitoraggio reattivo
    }
}


//fxnbr: 2

void print_argouments(t_argouments args)
{
    //printf(BLACK "BLACK" RESET);
    printf(RED "RED " RESET);
    printf(YELLOW "YELLOW " RESET);
    printf(GREEN "GREEN " RESET);
    printf(BLUE "BLUE " RESET);
    printf(MAGENTA "MAGENTA " RESET);
    printf(CYAN "SAYAN " RESET);
    //printf(WHITE "WHITE" RESET);
    puts("");

    printf("Number of philosophers: %d\n", args.philo_nbr);
    printf("\nTime to die: %d ms\nIl filosofo muore di fame se non mangia entro %d millisecondi dall'ultimo pasto (o dall'inizio)\n", args.time_to_die, args.time_to_die);
    printf("\nTime to eat: %d ms\nUn filosofo impiega %d millisecondi per mangiare (durante i quali detiene entrambe le forchette)\n", args.time_to_eat, args.time_to_eat);
    printf("\nTime to sleep: %d ms\nDopo aver mangiato, il filosofo dorme per %d millisecondi prima di tornare a pensare.\n", args.time_to_sleep, args.time_to_sleep);

    if (args.number_of_times_each_philosopher_must_eat == -1)
        printf("\nNumber of times each philosopher must eat: Not specified\n");
    else
        printf("\nNumber of times each philosopher must eat: %d\n", args.number_of_times_each_philosopher_must_eat);
}

void print_table(t_table *table)
{
    int i = 0;
    while (i < table->args.philo_nbr)
    {
        printf("Filosofo %d ha la forchetta %d (sx) e la forchetta %d (dx)\n",
               table->philos[i].id,
               table->philos[i].left_fork->fork_id,
               table->philos[i].right_fork->fork_id);
        i++;
    }
}