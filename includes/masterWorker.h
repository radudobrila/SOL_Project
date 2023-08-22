#ifndef MASTER_WORKER_H

    #define MASTER_WORKER_H

    struct Queue{
        char *file;
        char *path;
        struct Queue *next;
    };

#endif