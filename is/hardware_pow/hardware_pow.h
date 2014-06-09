/**
 * @file      ac_tlm_mem.h
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
 * @brief     Defines a ac_tlm memory.
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
#ifndef HARDWARE_POW_H_
#define HARDWARE_POW_H_

#define HARDWARE_POW_ADDR_BASE 5242884U
#define HARDWARE_POW_ADDR_EXPONENT 5242888U
#define HARDWARE_POW_ADDR_RESULT 5242892U

//////////////////////////////////////////////////////////////////////////////

// Standard includes
// SystemC includes
#include <systemc>
// ArchC includes
#include "ac_tlm_protocol.H"
#include  "ac_tlm_port.H"
#include  "ac_memport.H"

//////////////////////////////////////////////////////////////////////////////

// using statements
using tlm::tlm_transport_if;

//////////////////////////////////////////////////////////////////////////////

//#define DEBUG

/// Namespace to isolate memory from ArchC
namespace user
{

/// A TLM memory
class hardware_pow :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
private:
  int exponent;
  int base;
  int ipow(int base, int exp);
public:
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export;

  ac_tlm_rsp_status write_base(const uint32_t & );
  ac_tlm_rsp_status write_exponent(const uint32_t & );
  ac_tlm_rsp_status read_result(uint32_t & );

  /**
   * Implementation of TLM transport method that
   * handle packets of the protocol doing apropriate actions.
   * This method must be implemented (required by SystemC TLM).
   * @param request is a received request packet
   * @return A response packet to be send
  */
  ac_tlm_rsp transport( const ac_tlm_req &request ) {
    ac_tlm_rsp response;

    switch(request.addr)
    {
    case HARDWARE_POW_ADDR_BASE:
      if (request.type == READ)
        response.status = ERROR;
      else if (request.type == WRITE)
      {
        response.status = write_base( request.data );
      }
      break;
    case HARDWARE_POW_ADDR_EXPONENT:
      if (request.type == READ)
        response.status = ERROR;
      else if (request.type == WRITE)
      {
        response.status = write_exponent( request.data );
      }
      break;
    case HARDWARE_POW_ADDR_RESULT:
      if (request.type == WRITE)
        response.status = ERROR;
      else if (request.type == READ)
      {
        response.status = read_result( response.data );
      }
      break;
    }

    return response;
  }


  /**
   * Default constructor.
   *
   * @param k Memory size in kilowords.
   *
   */
  hardware_pow( sc_module_name module_name);

  /**
   * Default destructor.
   */
  ~hardware_pow();

};

};

#endif
