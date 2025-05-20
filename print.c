//fxnbr: 3
#include "philo.h"

bool should_terminate(t_print *print, t_message **tmp)
{
    bool end;

    if (*tmp)
    {
        free((*tmp)->string);
        free(*tmp);
        *tmp = NULL;
    }
    safe_get(&print->kill_mutex, &print->end, &end, sizeof(bool));
    if (end)
        return (true);
    else
        return (false);
}

int init_print(t_table *table)
{
    table->queue.head = NULL;
    table->queue.tail = NULL;
    table->queue.end  = false;

    if (pthread_mutex_init(&table->queue.mutex, NULL) != SUCCESS)
        return (FAILURE);
    return (SUCCESS);
}

int print_event(t_philo *philo, const char *str) // NOTE passagli msg->timestamp
{
    if (!philo)
    {
        fprintf(stderr, RED "Errore: print_even philo è NULL\n" RESET); //NOTE - debug
        return (FAILURE);
    }
    if (!str)
    {
        fprintf(stderr, RED "Errore: print_even stringa evento NULL per philo %d\n" RESET, philo->id); //NOTE - debug
        return (FAILURE);
    }
    // NOTE qua ci sono due timestamp, fai attenzione che nella stama prendi il timestamp effettivo
    long timestamp = get_time() - philo->table->start_time;
    return add_print_event(timestamp, philo, str);
}



//fxnbr: 4

void init_message_queue(t_queue *q)
{
    q->head = NULL;
    q->tail = NULL;
    q->end = false;
    pthread_mutex_init(&q->mutex, NULL);
}

t_message *create_print_message(t_philo *philo, long timestamp, const char *str) // NOTE passagli msg->timestamp
{
    if (!philo || philo->id <= 0 || !str)
    {
        fprintf(stderr, RED "DEBUG create_print_message: Messaggio malformato id: %d\n" RESET,
                philo ? philo->id : -1);
        return NULL;
    }

    t_message *msg = safe_alloc(1, sizeof(t_message), "print message");
    if (!msg)
        return NULL;

    msg->string = ft_strdup(str);
    if (!msg->string)
    {
        free(msg);
        return NULL;
    }
    // NOTE qua dovrebbe chiamare queue qua sto facendo due robe, devo returnare string->msg (messaggio non allocato in precedenza)
    msg->id = philo->id;
    msg->timestamp = timestamp;
    msg->next = NULL;

    return msg;
}


int add_print_event(long ms, t_philo *philo, const char *str) // NOTE passagli msg->timestamp
{
    if (!philo || philo->id <= 0 || !str)
    {
        printf("DEBUG add_print_event: Messaggio malformato id: %d\n", philo ? philo->id : -1); //NOTE - debug
        return (FAILURE);
    }
    t_message *new_msg = create_print_message(philo, ms, str);
    if (!new_msg)
        return (FAILURE);

    pthread_mutex_lock(&philo->table->queue.mutex);
    if (!philo->table->queue.tail)
        philo->table->queue.head = philo->table->queue.tail = new_msg;
    else
    {
        philo->table->queue.tail->next = new_msg;
        philo->table->queue.tail = new_msg;
    }
    pthread_mutex_unlock(&philo->table->queue.mutex);

    return (SUCCESS);
}

void *queue(void *arg) // NOTE passagli msg->timestamp
{
    bool end_flag = false;
    t_table *table = (t_table *)arg;
    t_message *msg;

    while (true)
    {
        safe_get(&table->supervisor.lock_end, &table->end, &end_flag, sizeof(bool));
        if (end_flag)
            break;

        pthread_mutex_lock(&table->queue.mutex);
        if (!table->queue.head)
        {
            pthread_mutex_unlock(&table->queue.mutex);
            usleep(42);
            continue;
        }

        msg = table->queue.head;
        table->queue.head = msg->next;
        if (!table->queue.head)
            table->queue.tail = NULL;
        pthread_mutex_unlock(&table->queue.mutex);

        // Validazione
        if (!msg || !msg->string || msg->id <= 0)
        {
            fprintf(stderr, YELLOW "DEBUG queue: Messaggio malformato (id=%d)\n" RESET,
                    msg ? msg->id : -1);
            if (msg)
            {
                free(msg->string);  // Protezione se msg->string era parzialmente allocata
                free(msg);
            }
            continue;
        }
        // NOTE alloca il messaggio con strcpy
        pretty_output(msg->timestamp, 7, msg->id, msg->string);
        free(msg->string);
        free(msg);
    }
    return NULL;
}



void stop_print(t_table *table)
{
    pthread_mutex_lock(&table->queue.mutex);
    table->end = true;
    pthread_mutex_unlock(&table->queue.mutex);
}


