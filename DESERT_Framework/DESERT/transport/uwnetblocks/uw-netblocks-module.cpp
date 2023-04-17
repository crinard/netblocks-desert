/**
 * @file   uwnetblocks-module.cpp
 * @author Chris Rinard
 *
 * \brief Provides the <i>UWNETBLOCKS</i> class implementation.
 *
 * Provides the <i>UWNETBLOCKS</i> class implementation.
 */

#include "uwnetblocks-module.h"

#include <iostream>
#include <set>

extern packet_t PT_UWNETBLOCKS;

int hdr_uwnetblocks::offset_ = 0; /**< Offset used to access in <i>hdr_uwnetblocks</i> packets header. */

/**
 * Adds the header for <i>hdr_uwnetblocks</i> packets in ns2.
 */
static class UwNetBlocksPktClass : public PacketHeaderClass
{
public:
	UwNetBlocksPktClass()
		: PacketHeaderClass("PacketHeader/UWNETBLOCKS", sizeof(hdr_uwnetblocks))
	{
		this->bind();
		bind_offset(&hdr_uwnetblocks ::offset_);
	}
} class_uwnetblocks_pkt;

/**
 * Adds the module for UwNetBlocksClass in ns2.
 */
static class UwNetBlocksClass : public TclClass
{
public:
	UwNetBlocksClass()
		: TclClass("Module/UW/NetBlocks")
	{
	}

	TclObject *
	create(int, const char *const *)
	{
		return (new UwNetBlocks);
	}
} class_module_uwnetblocks;

UwNetBlocks::UwNetBlocks()
	: portcounter(0)
	, drop_duplicated_packets_(0)
	, debug_(0)
{
	bind("drop_duplicated_packets_", &drop_duplicated_packets_);
	bind("debug_", &debug_);
}

UwNetBlocks::~UwNetBlocks()
{
}

int
UwNetBlocks::command(int argc, const char *const *argv)
{
	Tcl &tcl = Tcl::instance();
	if (argc == 2) {
		if (strcasecmp(argv[1], "getnetblocksheadersize") == 0) {
			tcl.resultf("%d", this->getNetBlocksHeaderSize());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "printidspkts") == 0) {
			this->printIdsPkts();
			return TCL_OK;
		}
	}
	if (argc == 3) {
		if (strcasecmp(argv[1], "assignPort") == 0) {
			Module *m = dynamic_cast<Module *>(tcl.lookup(argv[2]));
			if (!m)
				return TCL_ERROR;
			int port = assignPort(m);
			tcl.resultf("%d", port);
			return TCL_OK;
		}
	}
	return Module::command(argc, argv);
}

void
UwNetBlocks::recv(Packet *p)
{
std:
	cerr << "PortMap::recv() a Packet is sent without source module!"
		 << std::endl;
	Packet::free(p);
}

void
UwNetBlocks::recv(Packet *p, int idSrc)
{
	hdr_cmn *ch = HDR_CMN(p);
	hdr_uwnetblocks *uwnetblocks = HDR_UWNETBLOCKS(p);
	hdr_uwip *iph = HDR_UWIP(p);

	if (!ch->error()) {
		if (ch->direction() == hdr_cmn::UP) {

			map<int, int>::const_iterator iter = id_map.find(uwnetblocks->dport());

			if (iter == id_map.end()) { // Unknown Port Number
				if (debug_)
					std::cout << "UwNetBlocks::recv() (dir:UP) "
							  << " dport=" << uwnetblocks->dport()
							  << " portcounter=" << portcounter << std::endl;
				drop(p, 1, DROP_UNKNOWN_PORT_NUMBER);
				return;
			}

			int id = iter->second;

			if (drop_duplicated_packets_ == 1) {
				map<uint16_t, map_packets_el>::iterator it =
						map_packets.find(iter->first);
				if (debug_ > 10)
					std::cout << ch->uid() << ":"
							  << static_cast<uint16_t>(iph->saddr()) << ":"
							  << iter->first << std::endl;
				if (it == map_packets.end()) { // Packet to a "new" port.
					if (debug_ > 10)
						std::cout << "--> new port" << std::endl;
					std::set<int> tmp_set_;
					tmp_set_.insert(ch->uid());
					map_packets_el tmp_map_el_;
					tmp_map_el_.insert(pair<uint8_t, std::set<int> >(
							iph->saddr(), tmp_set_));
					map_packets.insert(pair<uint16_t, map_packets_el>(
							iter->first, tmp_map_el_));
				} else { // Port already used.
					if (debug_ > 10)
						std::cout << "--> old port" << std::endl;
					std::map<uint8_t, std::set<int> >::iterator it2 =
							it->second.find(iph->saddr());
					if (it2 == it->second.end()) { // Port already used but
												   // packet from a new source.
						if (debug_ > 10)
							std::cout << "  --> new source" << std::endl;
						std::set<int> tmp_set_;
						tmp_set_.insert(ch->uid());
						it->second.insert(pair<uint8_t, std::set<int> >(
								iph->saddr(), tmp_set_));
					} else { // Port already used and old source.
						if (debug_ > 10)
							std::cout << "  --> old source" << std::endl;
						if (it2->second.count(ch->uid()) < 1) {
							if (debug_ > 10)
								std::cout << "    --> new packet" << std::endl;
							it2->second.insert(ch->uid());
						} else { // Packet already received.
							if (debug_ > 10) {
								std::cout << "    --> duplicated packet"
										  << std::endl;
								std::cout << "    --> dropped" << std::endl;
							}
							drop(p, 1, DROP_RECEIVED_DUPLICATED_PACKET);
							return;
						}
					}
				}
			}

			ch->size() -= sizeof(hdr_uwnetblocks);
			sendUp(id, p);
		} else { // Direction DOWN.
			map<int, int>::const_iterator iter = port_map.find(idSrc);

			if (iter == port_map.end()) {
			std:
				cerr << "UwNetBlocks::recv() (dir:DOWN) no port assigned to id "
					 << idSrc << ", dropping packet!" << std::endl;
				Packet::free(p);
				return;
			}

			int sport = iter->second;
			assert(sport > 0 && sport <= portcounter);
			uwnetblocks->sport() = sport;

			ch->size() += sizeof(hdr_uwnetblocks);
			sendDown(p);
		}
	}
}

int
UwNetBlocks::assignPort(Module *m)
{
	int id = m->getId();

	// Check that the provided module has not been given a port before
	if (port_map.find(id) != port_map.end())
		return TCL_ERROR;

	int newport = ++portcounter;

	port_map[id] = newport;
	assert(id_map.find(newport) == id_map.end());
	id_map[newport] = id;
	assert(id_map.find(newport) != id_map.end());

	if (debug_) {
		std::cerr << "UwNetBlocks::assignPort() "
				  << " id=" << id << " port=" << newport
				  << " portcounter=" << portcounter << std::endl;
	}
	return newport;
}
