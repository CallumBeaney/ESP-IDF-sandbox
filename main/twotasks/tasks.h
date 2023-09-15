#ifndef TASKS_H
#define TASKS_H


void receiver(void * params);
void sender(void * params);
void runTwoTasksWithHandlers(void);

void task1(void * params);
void task2(void * params);
void handleTwoTasksConcurrently(void);

#endif