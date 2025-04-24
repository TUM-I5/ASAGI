/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "nonuma.h"
#include "numacomm.h"

/**
 * @copydoc numa::Numa::createComm
 */
numa::NumaComm* numa::NoNuma::createComm() const { return new NumaComm(*this); }
