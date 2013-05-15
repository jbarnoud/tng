/* This code is part of the tng compression routines.
 *
 * Written by Daniel Spangberg
 * Copyright (c) 2010, 2013, The GROMACS development team.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 */


#ifndef DICT_H
#define DICT_H

void Ptngc_comp_canonical_dict(unsigned int *dict, int *ndict);

void Ptngc_comp_make_dict_hist(unsigned int *vals, int nvals,
			 unsigned int *dict, int *ndict,
			 unsigned int *hist);

#endif
