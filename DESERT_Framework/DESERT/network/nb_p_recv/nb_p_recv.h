/**
 * @file   nb_p.h
 * @version 1.0.0
 */

#ifndef _NB_P_RECV_H_
#define _NB_P_RECV_H_

#define DROP_DEST_NO_ROUTE \
	"DNR" /**< Reason for a drop in a <i>UWVBR</i> module. */

#include "module.h"
#include "packet.h"
#include <map>
#include <vector>
#include <stdint.h> 

#include "nbp_runtime.h"
#include "nbp_gen_headers.h"

typedef enum type_sim_p {
	NOT_SET,
	CONTROL_STREAM
} type_sim_t_p;

namespace
{
static const uint16_t IP_ROUTING_MAX_ROUTES =
		254; /**< Maximum number of entries in the routing table of a node. */
}

/**
 * Nb_p_recv_Module class implements basic routing functionalities.
 */
class Nb_p_recv_Module : public Module
{
public:
	/**
	 * Constructor of Nb_p_recv_Module class.
	 */
	Nb_p_recv_Module();

	/**
	 * Destructor of Nb_p_recv_Module class.
	 */
	virtual ~Nb_p_recv_Module();

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

	// virtual std::vector<Packet*> getNBReadBuffer(void);
	void senddown(Packet* p, double delay) {
		sendDown(p, delay);
	}
	inline Packet** getRecvBuf(size_t* len) {
		*len = recvBufLen;
		return recvBuf;
	}
	void setRecvBufLen(size_t n) {
		recvBufLen = n;
	}
	int getRecvBufLen(void) {
		return recvBufLen;
	}

protected:
	// Timers for sending, TODO: Make netblocks timers here. 
	class uwSendTimerAppl : public TimerHandler
	{
	public:
		uwSendTimerAppl(Nb_p_recv_Module *m, bool isNbp)
			: TimerHandler()
		{
			m_ = m;
			isNbp_ = isNbp;
		}

		virtual ~uwSendTimerAppl()
		{
		}

	protected:
		virtual void expire(Event *e);
		Nb_p_recv_Module *m_;
		bool isNbp_;
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
	nbp__connection_t * conn;
#define READ_BUF_LEN 1000
	Packet** recvBuf;
	size_t recvBufLen;
	type_sim_t_p sim_type;
	virtual double getRecvBytes(void);
	virtual double getSentBytes(void);
	virtual bool set_mode_telem(void);
	double period_;
};
#define CONTROL_MSG ("KPAO KPAO RJAA May 07 2023 in N819LA DA40 DA40 B757 NULUK R220 NIPPI R220 NANAC OTR10 KAGIS BOSPA BOSPSB  B757 NULUK R220 NIPPI R220 NANAC OTR10 KAGIS BOSPA BOSPS")
#endif // _NB_P_H_
