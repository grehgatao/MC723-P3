/**
 * @file      ac_tlm_mem.cpp
 * @author    Bruno de Carvalho Albertini
 *
 * @author    The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   0.1
 * @date      Sun, 02 Apr 2006 08:07:46 -0200
 *
 * @brief     Implements a ac_tlm memory.
 *
 * @attention Copyright (C) 2002-2005 --- The ArchC Team
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *
 */

//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include "bar_mem.h"
#include  "ac_tlm_port.H"
#include  "ac_memport.H"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate memory from ArchC
using user::bar_mem;

/// Constructor
bar_mem::bar_mem( sc_module_name module_name) :
DM_port("DM_port", 5242880U),
target_export1("iport1"),
target_export2("iport2"),
target_export3("iport3"),
target_export4("iport4"),
target_export5("iport5"),
target_export6("iport6"),
target_export7("iport7"),
target_export8("iport8")
{
  /// Binds target_export to the memory
  target_export1( *this );
  target_export2( *this );
  target_export3( *this );
  target_export4( *this );
  target_export5( *this );
  target_export6( *this );
  target_export7( *this );
  target_export8( *this );
  write_lock = false;
}

/// Destructor
bar_mem::~bar_mem() {
}