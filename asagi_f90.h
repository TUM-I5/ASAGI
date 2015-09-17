/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
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
void f90asagi_grid_set_param(int grid_id, const char* name,
	const char* value, int level);
asagi_error f90asagi_grid_open(int grid_id, const char* filename,
	int level);

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
}	// end extern "C"
#endif

#endif // ifndef ASAGI_F95_H
