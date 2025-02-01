/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef ASAGI_F95_H
#define ASAGI_F95_H

#include "asagi.h"

#ifdef __cplusplus
extern "C" {
#endif

int f90asagi_grid_create(asagi_type type);

int f90asagi_grid_create_array(asagi_type basic_type);

int f90asagi_grid_create_struct(int count,
                                int blockLength[],
                                long displacements[],
                                asagi_type types[]);

void f90asagi_grid_set_comm(int grid_id, int comm);
void f90asagi_grid_set_threads(int grid_id, int threads);
void f90asagi_grid_set_param(int grid_id, const char* name, const char* value, int level);
asagi_error f90asagi_grid_open(int grid_id, const char* filename, int level);

int f90asagi_grid_dimensions(int grid_id);

double f90asagi_grid_min(int grid_id, int n);
double f90asagi_grid_max(int grid_id, int n);

double f90asagi_grid_delta(int grid_id, int n, int level);

int f90asagi_grid_var_size(int grid_id);

unsigned char f90asagi_grid_get_byte(int grid_id, double* pos, int level);
int f90asagi_grid_get_int(int grid_id, double* pos, int level);
long f90asagi_grid_get_long(int grid_id, double* pos, int level);
float f90asagi_grid_get_float(int grid_id, double* pos, int level);
double f90asagi_grid_get_double(int grid_id, double* pos, int level);
void f90asagi_grid_get_buf(int grid_id, void* buf, double* pos, int level);

void f90asagi_grid_close(int grid_id);

asagi_error f90asagi_start_comm_thread(int sched_cpu, int comm);
void f90asagi_stop_comm_thread();

int f90asagi_node_local_rank(int comm);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif // ifndef ASAGI_F95_H
