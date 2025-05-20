//fxnbr: 5
#include "philo.h"

void ft_putchar(char ch, int *c)
{
	write(1, &ch, 1);
	(*c)++;
}

void ft_putstr(char *str, int *c)
{
    if (!str)
        return ;
    while (*str)
    {
        ft_putchar(*str, c);
        str++;
    }
}


void ft_putnbr(long n, int *c)
{
	if (n < 0)
	{
		n = n * -1;
		ft_putchar('-', c);
	}
	if (n < 10)
		ft_putchar((char)(n + 48), c);
	else
	{
		ft_putnbr(n / 10, c);
		ft_putnbr(n % 10, c);
	}
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

void *ft_memcpy(void *dst, void *src, size_t n)
{
	unsigned char *d = (unsigned char *)dst;
    unsigned char *s = (unsigned char *)src;

    if (dst == NULL || src == NULL)
        return NULL;

    while (n-- > 0)
        *d++ = *s++;

    return dst;
}

size_t ft_strlen(const char *str)
{
	size_t len = 0;
	while (str && str[len])
		len++;
	return (len);
}

char *ft_strcpy(char *dst, const char *src)
{
	size_t i = 0;
	while (src && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

char *ft_strdup(const char *src)
{
	size_t len;
	char *dup;

	if (!src)
		return (NULL);
	len = ft_strlen(src);
	dup = safe_alloc(len + 1, sizeof(char), "ft_strdup string");
	if (!dup)
		return (NULL);
	return ft_strcpy(dup, src);
}



//fxnbr: 4

const char *set_color(const char *event)
{
    const char *color = RESET;

    if (ft_strcmp(event, "is thinking") == 0)
        color = BLUE;
    else if (ft_strcmp(event, "is eating") == 0)
        color = GREEN;
    else if (ft_strcmp(event, "is sleeping") == 0)
        color = MAGENTA;
    else if (ft_strcmp(event, "died") == 0)
        color = RED;
    // else if (ft_strcmp(event, "is full") == 0)
    //     color = CYAN;
	else if (ft_strcmp(event, "picked up left fork") == 0 || \
             ft_strcmp(event, "picked up right fork") == 0)
        color = YELLOW;
    // else if (ft_strcmp(event, "released left fork") == 0 || 
    //          ft_strcmp(event, "released right fork") == 0)
    //     color = ORANGE;  

    return color;
}

int ft_printf(const char *str, ...)
{
	va_list ptr;
	int i;
	int c;

	i = 0;
	c = 0;
	va_start(ptr, str);
	while (str[i] != '\0')
	{
		if (str[i] == '%')
		{
			ft_vcheck(str[i + 1], ptr, &c);
			i++;
		}
		else
			ft_putchar(str[i], &c);
		i++;
	}
	va_end(ptr);
	return (c);
}

void ft_vcheck(const char v, va_list ptr, int *c)
{
	if (v == 's')
		ft_putstr((va_arg(ptr, char *)), c);
	if (v == 'd' || v == 'i')
		ft_putnbr(va_arg(ptr, int), c);
	if (v == '%')
		ft_putchar('%', c);
}

void pretty_output(long time_ms, int max_width, int philosopher_id, const char *event)
{
    int   num_digits = 0;
    long  temp = time_ms;
    int   num_dots;
    int   i;
    const char *color = set_color(event);

	if (!event)
    {
        fprintf(stderr, RED "Errore: evento NULL\n" RESET); //NOTE - debug
        return;
    }
    if (time_ms == 0)
    {
        ft_printf("%s......0      id:%d      %s\n" RESET, color, philosopher_id, event);
        return;
    }
    while (temp > 0)
    {
        temp /= 10;
        num_digits++;
    }
    num_dots = max_width - num_digits;
    i = 0;
    while (i++ < num_dots)
        ft_printf(".");
    ft_printf("%s%ld      id:%d    \t%s\n" RESET, color, time_ms, philosopher_id, event);
}

//fxnbr: 3

unsigned long get_time(void)
{
	struct timeval	tv;
	unsigned long	time;

	time = 0;
	if (gettimeofday(&tv, NULL))
		return (ft_printf("Error: getting the time of day\n"), 0);
	time = (tv.tv_sec * (unsigned long)1000) + (tv.tv_usec / 1000);
	return (time);
}

void set_end_time(t_table *table, bool value)
{
    safe_set(&table->supervisor.lock_end, &value, &table->end, sizeof(bool));
}

int get_end_time(t_table *table)
{
    bool result;
    safe_get(&table->supervisor.lock_end, &table->end, &result, sizeof(bool));
    return result;
}

