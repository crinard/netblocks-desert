/**
 * @file   nb_p.h
 * @version 1.0.0
 */

#ifndef _NB_P_H_
#define _NB_P_H_

#define DROP_DEST_NO_ROUTE \
	"DNR" /**< Reason for a drop in a <i>UWVBR</i> module. */

#include "module.h"
#include "packet.h"
#include <map>
#include <vector>
#include <stdint.h> 

#include "nb_runtime.h"
#include "gen_headers.h"

typedef enum type_sim {
	NOT_SET,
	VIDEO_STREAM,
	CONTROL_STREAM
} type_sim_t;

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
		uwSendTimerAppl(Nb_pModule *m, bool isNb)
			: TimerHandler()
		{
			m_ = m;
			isNb_ = isNb;
		}

		virtual ~uwSendTimerAppl()
		{
		}

	protected:
		virtual void expire(Event *e);
		Nb_pModule *m_;
		bool isNb_;
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
	virtual double getHeaderSize(void);
	virtual bool set_mode_telem(void);
	virtual bool set_mode_video(void);
	virtual void set_compressed(bool setCmpr_);
	virtual char* genTelemPkt(int* size);
	virtual char* genVideoPkt(int* size);
	virtual double getRecvBytes(void);
	virtual double getSentBytes(void);
	uwSendTimerAppl chkTimerPeriod;
	uwSendTimerAppl chkNetBlocksTimer;
	type_sim_t sim_type;
#define READ_BUF_LEN 1000
	nb__connection_t * conn;
	Packet** recvBuf;
	size_t recvBufLen;
	bool is_compressed;
};

#endif // _NB_P_H_
