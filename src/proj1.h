/*
 * proj1.h
 *
 *  Created on: Apr 5, 2014
 *      Author: daniel
 */

#ifndef PROJ1_H_
#define PROJ1_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


void sigusr1Handler();

void monitorAux(char *filename);

void monitorWord(char *word, char *filename);

void setPrGrID(pid_t group, pid_t proc);

#endif /* PROJ1_H_ */
