//fxnbr: 3
#include "philo.h"

static void init_forks(t_table *table)
{
	int i = 0;

	while (i < table->args.philo_nbr)
	{
		pthread_mutex_init(&table->fork[i].fork, NULL);
		table->fork[i].fork_id = i + 1;
		i++;
	}
}

void init_table(t_table *table)
{
	int i;

    i = 0;
    table->start_time = time(NULL);
    table->end = false;
	table->fork = safe_alloc(table->args.philo_nbr, sizeof(t_fork), "fork");
    table->philos = safe_alloc(table->args.philo_nbr, sizeof(t_philo), "philo");
	init_forks(table);
    while (i < table->args.philo_nbr)
    {
        table->philos[i].id = i + 1;
        table->philos[i].number_of_meals_eaten = 0;
        table->philos[i].last_meal_time = 0;
        table->philos[i].full_stomach = false;
        table->philos[i].rogue = false;
        table->philos[i].left_fork = &table->fork[i];
        table->philos[i].right_fork = &table->fork[(i + 1) % table->args.philo_nbr];
        table->philos[i].table = table;
        i++;
    }
    init_supervisor_mutexes(table);
}

int init_philos(t_table *table)
{
    int i;

    i = 0;
    table->start_time = get_time();
    while (i < table->args.philo_nbr)
    {
        table->philos[i].last_meal_time = table->start_time;
        if (pthread_create(&table->philos[i].thread, NULL, routine, &table->philos[i]) != SUCCESS)
        {
            ft_printf("Errore: creazione thread fallita per filosofo %d.\n", i + 1);
            return (FAILURE);
        }
        i++;
    }
    return (SUCCESS);
}

int init_supervisor_mutexes(t_table *table)
{
    if (pthread_mutex_init(&table->supervisor.lock_last_meal_time, NULL) != 0)
        return (FAILURE);
    if (pthread_mutex_init(&table->supervisor.lock_number_meals_eaten, NULL) != 0)
        return (FAILURE);
    if (pthread_mutex_init(&table->supervisor.lock_full_stomach, NULL) != 0)
        return (FAILURE);
    if (pthread_mutex_init(&table->supervisor.lock_end, NULL) != 0)
        return (FAILURE);
    return (SUCCESS);
}


int init_supervisors(t_table *table)
{
	int i;

	i = 0;
	while (i < table->args.philo_nbr)
	{
		if (pthread_create(&table->philos[i].supervisor_thread,
			NULL, supervisor, &table->philos[i]) != 0)
		{
			ft_printf("Error: supervisor thread creation failed for philo %d\n", i + 1);
			return (FAILURE);
		}
		i++;
	}
	return SUCCESS;
}


//fxnbr: 2

int start_simulation(t_table *table)
{
    t_table *data = table;

    init_table(data); // inizializza la struttura di base e alloca memoria

    if (init_print(data) == FAILURE)            // 🔁 deve venire prima!
        return (FAILURE);

    if (init_supervisor_mutexes(data) == FAILURE) // 🔁 anche il supervisor va inizializzato prima
        return (FAILURE);

    if (init_philos(data) == FAILURE)            // ora puoi creare i filosofi e i thread
        return (FAILURE);

    if (launch_print(data) == FAILURE)           // e infine la thread per la stampa
        return (FAILURE);

    return (SUCCESS);
}


int launch_print(t_table *table)
{
    if (pthread_create(&table->queue.thread, NULL, queue, table) != SUCCESS)
        return (FAILURE);
    if (pthread_detach(table->queue.thread) != SUCCESS)
        return (FAILURE);
    return (SUCCESS);
}
