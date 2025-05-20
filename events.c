#include "philo.h"

void think(t_philo *philo)
{
    print_event(philo, "is thinking");
}

void philo_take_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(&philo->right_fork->fork);
        print_event(philo, "picked up right fork");
        pthread_mutex_lock(&philo->left_fork->fork);
        print_event(philo, "picked up left fork");
    }
    else
    {
        pthread_mutex_lock(&philo->left_fork->fork);
        print_event(philo, "picked up left fork");
        pthread_mutex_lock(&philo->right_fork->fork);
        print_event(philo, "picked up right fork");
    }
}

void philo_release_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_unlock(&philo->right_fork->fork);
        pthread_mutex_unlock(&philo->left_fork->fork);
    }
    else
    {
        pthread_mutex_unlock(&philo->left_fork->fork);
        pthread_mutex_unlock(&philo->right_fork->fork);
    }
}

int philo_sleep(t_philo *philo)
{
    if (print_event(philo, "is sleeping") == FAILURE)
        return FAILURE;
    usleep(philo->table->args.time_to_sleep * 1000);
    return SUCCESS;
}

void add_meal(t_philo *philo)
{
    int meals;
    bool full = true;

    safe_get(&philo->table->supervisor.lock_number_meals_eaten,
             &philo->number_of_meals_eaten, &meals, sizeof(int));
    meals++;
    safe_set(&philo->table->supervisor.lock_number_meals_eaten,
             &meals, &philo->number_of_meals_eaten, sizeof(int));

    if (meals >= philo->table->args.number_of_times_each_philosopher_must_eat)
        safe_set(&philo->table->supervisor.lock_full_stomach,
                 &full, &philo->full_stomach, sizeof(bool));
}

int eat(t_philo *philo)
{
    long meal_time;

    if (philo->table->args.philo_nbr == 1)
        return handle_single_philo(philo);

    if (lock_unlock_forks(philo, 1) == FAILURE)
        return FAILURE;

    meal_time = get_time() - philo->table->start_time;
    safe_set(&philo->table->supervisor.lock_last_meal_time,
             &meal_time, &philo->last_meal_time, sizeof(long));

    if (print_event(philo, "is eating") == FAILURE)
    {
        lock_unlock_forks(philo, 0);
        return FAILURE;
    }

    usleep(philo->table->args.time_to_eat * 1000);

    if (lock_unlock_forks(philo, 0) == FAILURE)
        return FAILURE;

    if (philo->table->args.number_of_times_each_philosopher_must_eat > 0)
        add_meal(philo);

    return SUCCESS;
}

int handle_single_philo(t_philo *philo)
{
    print_event(philo, "has taken a fork");
    usleep(philo->table->args.time_to_die * 1000);
    print_event(philo, "died");
    return FAILURE;
}

//fxnbr: 5

bool check_death(t_philo *philo, long now, long last_meal)
{
    if (now - last_meal > philo->table->args.time_to_die)
    {
        print_event(philo, "is dead");
        safe_set(&philo->table->supervisor.lock_end,
                 &(bool){true}, &philo->table->end, sizeof(bool));
        return true;
    }
    return (false);
}

int check_end(t_philo *philo)
{
    bool end;

    safe_get(&philo->table->supervisor.lock_end, &philo->table->end, &end, sizeof(bool));
    if (end == true)
        return (FAILURE);
    return (SUCCESS);
}

void wait_for_threads(t_table *table)
{
	int i;

	i = 0;
	while (i < table->args.philo_nbr)
	{
		pthread_join(table->philos[i].thread, NULL);
		i++;
	}
	i = 0;
	while (i < table->args.philo_nbr)
	{
		pthread_join(table->philos[i].supervisor_thread, NULL);
		i++;
	}
}

void cleanup(t_table *table)
{
    int i;
    t_message *msg;
    
    i = 0;
    while (i < table->args.philo_nbr)
        pthread_mutex_destroy(&table->fork[i++].fork);
    free(table->philos);
    free(table->fork);
    pthread_mutex_destroy(&table->supervisor.lock_last_meal_time);
    pthread_mutex_destroy(&table->supervisor.lock_number_meals_eaten);
    pthread_mutex_destroy(&table->supervisor.lock_full_stomach);
    pthread_mutex_destroy(&table->supervisor.lock_end);
    pthread_mutex_destroy(&table->queue.mutex);
    while (table->queue.head)
    {
        msg = table->queue.head;
        table->queue.head = msg->next;
        free(msg->string);
        free(msg);
    }
}

void end_simulation(t_table *table)
{
	int i;

	i = 0;
	while (i < table->args.philo_nbr)
		pthread_join(table->philos[i++].thread, NULL);
	i = 0;
	while (i < table->args.philo_nbr)
		pthread_join(table->philos[i++].supervisor_thread, NULL);
	cleanup(table);
}


//fxnbr: 2

void safe_set(pthread_mutex_t *mutex, void *dst, void *src, size_t size)
{
    if (mutex == NULL || dst == NULL || src == NULL)
        return;

    pthread_mutex_lock(mutex);
    ft_memcpy(dst, src, size);
    pthread_mutex_unlock(mutex);
}

void safe_get(pthread_mutex_t *mutex, void *src, void *dst, size_t size)
{
    if (mutex == NULL || src == NULL || dst == NULL)
        return;

    pthread_mutex_lock(mutex);
    ft_memcpy(dst, src, size);
    pthread_mutex_unlock(mutex);
}

//fxnbr: 3

void    ft_bzero(void *s, size_t n)
{
	size_t  i;
	char    *str;

	i = 0;
	str = s;
	while (i < n)
	{
		str[i] = '\0';
		i++;
	}
}

void    *ft_calloc(size_t number_of_elements, size_t size)
{
	void    *array;

	array = (void *)malloc(size * number_of_elements);
	if (array == NULL)
		return (NULL);
	ft_bzero(array, (size * number_of_elements));
	return (array);
}

void    *safe_alloc(size_t n_elem, size_t bytes, const char *description)
{
    void *memory_allocation;

    memory_allocation = ft_calloc(n_elem, bytes);
    if (memory_allocation == NULL)
    {
        ft_printf("Error: memory allocation for [%s] has failed\n", description);
        return (NULL);
    }
    return (memory_allocation);
}

int lock_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(&philo->right_fork->fork);
        print_event(philo, "picked up right fork");
        pthread_mutex_lock(&philo->left_fork->fork);
        print_event(philo, "picked up left fork");
    }
    else
    {
        pthread_mutex_lock(&philo->left_fork->fork);
        print_event(philo, "picked up left fork");
        pthread_mutex_lock(&philo->right_fork->fork);
        print_event(philo, "picked up right fork");
    }
    return (SUCCESS);
}

int unlock_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_unlock(&philo->right_fork->fork);
        pthread_mutex_unlock(&philo->left_fork->fork);
    }
    else
    {
        pthread_mutex_unlock(&philo->left_fork->fork);
        pthread_mutex_unlock(&philo->right_fork->fork);
    }
    return (SUCCESS);
}

int lock_unlock_forks(t_philo *philo, int flag)
{
    if (flag == 0)
        return unlock_forks(philo);
    else if (flag == 1)
        return lock_forks(philo);
    return (FAILURE);
}


