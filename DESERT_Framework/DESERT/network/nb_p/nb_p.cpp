/**
 * @file   nb_p.cpp
 * @version 1.0.0
 *
 * \brief Provides the class implementation of NB_P.
 *
 * Provides the class implementation of NB_P.
 */

#include <stdlib.h>
#include <iostream>

#include "nb_p.h"

/**
 * Adds the module for Nb_pModuleClass in ns2.
 */
static class Nb_pModuleClass : public TclClass
{
public:
	Nb_pModuleClass()
		: TclClass("Module/UW/Nb_p")
	{
	}

	TclObject *
	create(int, const char *const *)
	{
		return (new Nb_pModule);
	}
} class_nb_p_module;

Nb_pModule::Nb_pModule()
	: default_gateway(0)
{
}

Nb_pModule::~Nb_pModule()
{
}

int
Nb_pModule::recvSyncClMsg(ClMessage *m)
{
	return Module::recvSyncClMsg(m);
}

int
Nb_pModule::command(int argc, const char *const *argv)
{
	Tcl &tcl = Tcl::instance();
	
	return Module::command(argc, argv);
}

void
Nb_pModule::recv(Packet *p)
{
	// std::cout << "NB_P::RECV" << std::endl;
	hdr_cmn *ch = HDR_CMN(p);
	std::cout << "NB_P::RECV::DIRECTION::" << ch->direction() << std::endl;
	// hdr_uwip *uwiph = HDR_UWIP(p);

	// if (debug_) {
	// 	std::cout << NOW << "::NB_P::RECV"
	// 			  << "::NEXT_HOP::" << ch->next_hop()
	// 			  << "::DESTINATION_IP::" << (uint)uwiph->daddr()
	// 			  << std::endl;
	// }

	// if (ch->direction() == hdr_cmn::UP) {

	// 	if (uwiph->daddr() == ch->next_hop() ||
	// 			uwiph->daddr() ==
	// 					UWIP_BROADCAST) { /* Packet is arrived at its
	// 										 destination */
	// 		sendUp(p);
	// 		return;
	// 	}
	// 	/* Forward Packet */
	// 	ch->direction() = hdr_cmn::DOWN;
	// 	ch->next_hop() = getNextHop(p);
	// 	if (ch->next_hop() == 0) {
	// 		drop(p, 1, DROP_DEST_NO_ROUTE);
	// 	} else {
	// 		sendDown(p);
	// 	}
	// } else { /* direction DOWN */
	// 	ch->next_hop() = getNextHop(p);
	// 	if (ch->next_hop() == 0) {
	// 		drop(p, 1, DROP_DEST_NO_ROUTE);
	// 	} else {
	// 		sendDown(p);
	// 	}
	// }
	return;
}
