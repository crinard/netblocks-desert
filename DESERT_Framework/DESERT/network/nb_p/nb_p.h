/**
 * @file   nb_p.h
 * @version 1.0.0
 */

#ifndef _NB_P_H_
#define _NB_P_H_

#define DROP_DEST_NO_ROUTE \
	"DNR" /**< Reason for a drop in a <i>UWVBR</i> module. */

#include <module.h>
#include <packet.h>
#include <map>
#include <vector>

#include "net-blocks/runtime/nb_runtime.h"

namespace
{
static const uint16_t IP_ROUTING_MAX_ROUTES =
		254; /**< Maximum number of entries in the routing table of a node. */
}

/**
 * Nb_pModule class implements basic routing functionalities.
 */
class Nb_pModule : public Module
{
public:
	/**
	 * Constructor of Nb_pModule class.
	 */
	Nb_pModule();

	/**
	 * Destructor of Nb_pModule class.
	 */
	virtual ~Nb_pModule();

	/**
	 * Performs the reception of packets from upper and lower layers.
	 *
	 * @param Packet* Pointer to the packet will be received.
	 */
	virtual void recv(Packet *);

	/**
	 * Cross-Layer messages synchronous interpreter.
	 *
	 * @param ClMessage* an instance of ClMessage that represent the message
	 * received
	 * @return <i>0</i> if successful.
	 */
	virtual int recvSyncClMsg(ClMessage *);

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

	virtual vector<Packet*> getNBReadBuffer(void);
protected:
	// Timers for sending, TODO: Make netblocks timers here. 
	class uwSendTimerAppl : public TimerHandler
	{
	public:
		uwSendTimerAppl(Nb_pModule *m)
			: TimerHandler()
		{
			m_ = m;
		}

		virtual ~uwSendTimerAppl()
		{
		}

	protected:
		virtual void expire(Event *e);
		Nb_pModule *m_;
	}; // End uwSendTimer class

	// Functions to start and stop generation
	virtual void start_gen(void);
	virtual void stop_gen(void);

	virtual void sendPkt(void);
	// Reporting functions
	virtual double getSentPkts(void);
	virtual double getRecvPkts(void);
	virtual double getDropPkts(void);
	virtual double getDelay(void);
	virtual double getThroughput(void);
	virtual double getHeaderSize(void);
	uwSendTimerAppl chkTimerPeriod;
	uwSendTimerAppl chkNetBlocksTimer;
	vector<Packet *> pktBuffer;
	nb__connection_t * conn;
};

#endif // _NB_P_H_
