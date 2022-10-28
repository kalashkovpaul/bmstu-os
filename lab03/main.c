// #include <unistd.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <syslog.h>
// #include <string.h>
// #include <errno.h>
// #include <stdio.h>
// #include <sys/stat.h>
// #include "apue.h"
// #include <pthread.h>
// #include <sys/resource.h>
// #include <signal.h>
#include "apue.h"
#include <pthread.h>
#include <syslog.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t mask;

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK; // блокировка на запись
    fl.l_start = 0;  // смещение относительно WHENCE, начало блокировки
    fl.l_whence = SEEK_SET; // курсор на начало
    fl.l_len = 0; // длина блокируемого участка
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(void)
{
    int fd;
    char buf[16];
    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        syslog(LOG_ERR, "невозможно открыть %s: %s",
        LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "невозможно установить блокировку на %s: %s",
        LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return(0);
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    /*
    * 1. Сбросить маску режима создания файла.
    */
    umask(0);

    /*
    * Получить максимально возможный номер дескриптора файла.
    */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        perror("невозможно получить максимальный номер дескриптора ");
    /*
    * 2. Стать лидером нового сеанса, чтобы утратить управляющий терминал.
    */
    if ((pid = fork()) < 0)
        perror("ошибка вызова функции fork");
    else if (pid != 0) /* родительский процесс */
        exit(0);

	/*
    * 3. Вызвать setsid()
    */

    if (setsid() == -1)
    {
        syslog(LOG_ERR, "невозможно сделать процесс лидером сессии и группы: %s", strerror(errno));
        exit(1);
    }
    /*
    * Обеспечить невозможность обретения управляющего терминала в будущем.
    */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        perror("невозможно игнорировать сигнал SIGHUP");

    /*
    * 4. Назначить корневой каталог текущим рабочим каталогом,
    * чтобы впоследствии можно было отмонтировать файловую систему.
    */
    if (chdir("/") < 0)
        perror("невозможно сделать текущим рабочим каталогом ");
    /*
    * 5. Закрыть все открытые файловые дескрипторы.
    */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    /*
    * Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
    */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    /*
    * 6. Инициализировать файл журнала.
    */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d",fd0, fd1, fd2);
        exit(1);
    }
}

// void *thr_fn(void *arg)
// {
//     int err, signo;
//     for (;;) {
//         err = sigwait(&mask, &signo);
//         if (err != 0) {
//             syslog(LOG_ERR, "ошибка вызова функции sigwait");
//             exit(1);
//         }
//         switch (signo) {
//             case SIGHUP:
//                 syslog(LOG_INFO, "Чтение конфигурационного файла");
//                 reread();
//                 break;
//             case SIGTERM:
//                 syslog(LOG_INFO, "получен сигнал SIGTERM; выход");
//                 exit(0);
//             default:
//                 syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
//         }
//     }
//     return(0);
// }

void *daemon_body(void *arg)
{
    long int time_d;
    while(1)
    {
        time_d = time(NULL);
        syslog(LOG_INFO, "Демон, время: %s\n", ctime(&time_d));
        sleep(5);
    }
}

int main(int argc, char *argv[])
{
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;
    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;
    /*
    * Перейти в режим демона.
    */
    daemonize(cmd);
    /*
    * Убедиться, что ранее не была запущена другая копия демона.
    */
    if (already_running()) {
        syslog(LOG_ERR, "демон уже запущен");
        exit(1);
    }
    /*
    * Восстановить действие по умолчанию для сигнала SIGHUP
    * и заблокировать все сигналы.
    */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: невозможно восстановить действие SIG_DFL для SIGHUP");
    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
        err_exit(err, "ошибка выполнения операции SIG_BLOCK");
    /*
    * Создать поток для обработки SIGHUP и SIGTERM.
    */
    err = pthread_create(&tid, NULL, daemon_body, 0);
    if (err != 0)
        err_exit(err, "невозможно создать поток");

    exit(0);
}
