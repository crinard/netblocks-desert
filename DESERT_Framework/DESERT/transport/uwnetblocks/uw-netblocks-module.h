/**
 * @file   uw-netblocks-module.h
 * @author Chris Rinard
 * @version 1.0.0
 *
 * \brief Provides the <i>UWNETBLOCKS</i> packets header description and the
 * definition of the class <i>UWNETBLOCKS</i>.
 *
 * <i>UWNETBLOCKS</i> implements a very simple module used to manage different
 * application flows.
 */

#ifndef _UWNETBLOCKS_H_
#define _UWNETBLOCKS_H_ 

#include <uwip-module.h>

#include <module.h>
#include <map>
#include <set>

#define DROP_UNKNOWN_PORT_NUMBER \
	"UPN" /**< Reason for a drop in a <i>UWNETBLOCKS</i> module. */
#define DROP_RECEIVED_DUPLICATED_PACKET \
	"RDP" /**< Reason for a drop in a <i>UWNETBLOCKS</i> module. */

#define HDR_UWNETBLOCKS(P) (hdr_uwnetblocks::access(P))

extern packet_t PT_UWNETBLOCKS;

/**
 * <i>hdr_uwnetblocks</i> describes <i>UWNETBLOCKS</i> packets.
 */
typedef struct hdr_uwnetblocks {
	uint8_t sport_; /**< Source port number. */
	uint8_t dport_; /**< Destination port number. */

	static int offset_; /**< Required by the PacketHeaderManager. */

	/**
	 * Reference to the offset_ variable.
	 */
	inline static int &
	offset()
	{
		return offset_;
	}

	inline static struct hdr_uwnetblocks *
	access(const Packet *p)
	{
		return (struct hdr_uwnetblocks *) p->access(offset_);
	}

	/**
	 * Reference to the sport_ variable.
	 */
	inline uint8_t &
	sport()
	{
		return sport_;
	}

	/**
	 * Reference to the dport_ variable.
	 */
	inline uint8_t &
	dport()
	{
		return dport_;
	}
} hdr_uwnetblocks;

/**
 * UwNetBlocks class is used to manage UWNETBLOCKS packets, and flows to and from upper
 * modules.
 */
class UwNetBlocks : public Module
{
public:
	/**
	 * Constructor of UwNetBlocks class.
	 */
	UwNetBlocks();
	/**
	 * Destructor of UwNetBlocks class.
	 */
	virtual ~UwNetBlocks();

	/**
	 * Performs the reception of packets from upper and lower layers.
	 *
	 * @param Packet* Pointer to the packet will be received.
	 */
	virtual void recv(Packet *);

	/**
	 * Performs the reception of packets from upper and lower layers.
	 *
	 * @param Packet* Pointer to the packet will be received.
	 * @param Handler* Handler.
	 */
	virtual void recv(Packet *, int);

	/**
	 * TCL command interpreter. It implements the following OTcl methods:
	 *
	 * @param argc Number of arguments in <i>argv</i>.
	 * @param argv Array of strings which are the command parameters (Note that
	 * <i>argv[0]</i> is the name of the object).
	 * @return TCL_OK or TCL_ERROR whether the command has been dispatched
	 * successfully or not.
	 *
	 */
	virtual int command(int, const char *const *);

	/**
	 * Associates a module with a port.
	 *
	 * @param Module* Module to which associate a port.
	 * @return Id of the port associated to the module.
	 */
	virtual int assignPort(Module *);

	/**
	 * Prints the IDs of the packet's headers defined by UWNETBLOCKS.
	 */
	inline void
	printIdsPkts() const
	{
		std::cout << "UWNETBLOCKS packets IDs:" << std::endl;
		std::cout << "PT_UWNETBLOCKS: \t\t" << PT_UWNETBLOCKS << std::endl;
	}

protected:
	uint16_t portcounter; /**< Counter used to generate new port numbers. */
	map<int, int> port_map; /**< Map: value = port;  key = id. */
	map<int, int> id_map; /**< Map: value = id;    key = port. */

	typedef std::map<uint8_t, std::set<int> > map_packets_el;
	std::
			map<uint16_t, map_packets_el>
					map_packets; /**< Map used to keep track of the packets
									received by each port. The key is the port
									number. The second element
															contains the saddr
									IP and a vector used as bucketlist.*/

	int drop_duplicated_packets_; /**< Flat to enable or disable the drop of
									 duplicated packets. */
	int debug_; /**< Flag to enable or disable dirrefent levels of debug. */

	/**
	 * Returns the size in byte of a <i>hdr_uwnetblocks</i> packet header.
	 *
	 * @return The size of a <i>hdr_uwnetblocks</i> packet header.
	 */
	static inline int
	getNetBlocksHeaderSize()
	{
		return sizeof(hdr_uwnetblocks);
	}
};

#endif // _UWNETBLOCKS_H_
