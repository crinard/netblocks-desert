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
	clearRoutes();
}

Nb_pModule::~Nb_pModule()
{
}

int
Nb_pModule::recvSyncClMsg(ClMessage *m)
{
	return Module::recvSyncClMsg(m);
}

void
Nb_pModule::clearRoutes()
{
	routing_table.clear();
}

void
Nb_pModule::addRoute(const uint8_t &dst, const uint8_t &next)
{
	// if (dst == 0 || next == 0) {
	// 	std::cerr << "You are trying to insert an invalid entry in the routing "
	// 				 "table with destination: "
	// 			  << static_cast<uint32_t>(dst)
	// 			  << " and next hop: " << static_cast<uint32_t>(next)
	// 			  << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
	// std::map<uint8_t, uint8_t>::iterator it = routing_table.find(dst);
	// if (it != routing_table.end()) {
	// 	it->second = next;
	// 	return;
	// } else {
	// 	if (routing_table.size() < IP_ROUTING_MAX_ROUTES) {
	// 		routing_table.insert(std::pair<uint8_t, uint8_t>(dst, next));
	// 		return;
	// 	} else {
	// 		std::cerr << "The routing table is full!" << std::endl;
	// 		return;
	// 	}
	// }
	return;
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

uint8_t
Nb_pModule::getNextHop(const Packet *p) const
{
	// hdr_uwip *uwiph = HDR_UWIP(p);
	// return getNextHop(uwiph->daddr());
	return 0;
}

uint8_t
Nb_pModule::getNextHop(const uint8_t &dst) const
{
	// std::map<uint8_t, uint8_t>::const_iterator it = routing_table.find(dst);
	// if (it != routing_table.end()) {
	// 	return it->second;
	// } else {
	// 	if (default_gateway != 0) {
	// 		return default_gateway;
	// 	} else {
	// 		return 0;
	// 	}
	// }
	return 0;
}
