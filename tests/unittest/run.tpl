/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 *
 * @brief Workaround for own main function
 *
 * The old CxxTest version does not allow to change the name of the main
 * function. However, we need our own main, get MPI working.
 * The main function is defined in {@link globaltest.h}.
 */

<CxxTest preamble>
<CxxTest world>
