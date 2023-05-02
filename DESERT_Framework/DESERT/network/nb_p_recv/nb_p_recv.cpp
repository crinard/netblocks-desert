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

#include "nb_p_recv.h"
#include "nbp_runtime.h"
extern char nbp__my_host_id[];
// static bool isrunning = 0;
/**
 * Adds the module for Nb_p_recv_ModuleClass in ns2.
 */
static class Nb_p_recvModuleClass : public TclClass
{
public:
	Nb_p_recvModuleClass()
		: TclClass("Module/UW/Nb_p_recv")
	{
	}

	TclObject *
	create(int, const char *const *)
	{
		return (new Nb_p_recv_Module);
	}
} class_nb_p_recv_module;
int running = 1;
static void callback(int event, nbp__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[65];
		int len = nbp__read(c, buff, 64);
		buff[len] = 0;	
		fprintf(stderr, "Client received = %s from server\n", buff);	
		running = 0;
	}
}

Nb_p_recv_Module::Nb_p_recv_Module():chkTimerPeriod(this, false), chkNetBlocksTimer(this, true) {
	// std::cout << "Nb_p RECIEVE MODULE INIT" << std::endl;
	nbp__desert_init((void*)this);
	nbp__net_init();
	char server_id[] = {0, 0, 0, 0, 0, 1};
	char client_id[] = {0, 0, 0, 0, 0, 2};
	std::cout << "server_id = " << server_id << std::endl;
	memcpy(nbp__my_host_id, client_id, 6);
	std::cout << "nbp__my_host_id = " << nbp__my_host_id << std::endl;
	conn = nbp__establish(server_id, 8080, 8081, callback);
	std::cout << "conn = " << conn << std::endl;

	chkNetBlocksTimer.resched(10.0);
	chkTimerPeriod.resched(120.0);

	recvBuf = (Packet**) calloc(READ_BUF_LEN, sizeof(Packet*));
	recvBufLen = 0;
}

Nb_p_recv_Module::~Nb_p_recv_Module(){
	nbp__destablish(conn);
	chkNetBlocksTimer.force_cancel();
	chkTimerPeriod.force_cancel();
}

int Nb_p_recv_Module::recvSyncClMsg(ClMessage *m)
{
	return Module::recvSyncClMsg(m);
}

int Nb_p_recv_Module::command(int argc, const char *const *argv)
{
	Tcl &tcl = Tcl::instance();
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			start_gen();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "stop") == 0) {
			stop_gen();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getsentpkts") == 0) {
			tcl.resultf("%f", getSentPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getrecvpkts") == 0) {
			tcl.resultf("%f", getRecvPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getdroppkts") == 0) {
			tcl.resultf("%f", getDropPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getdelay") == 0) {
			tcl.resultf("%f", getDelay());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getthroughput") == 0) {
			tcl.resultf("%f", getThroughput());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getheadersize") == 0) {
			tcl.resultf("%f", getHeaderSize());
			return TCL_OK;
		}
	}
	return Module::command(argc, argv);
}

void Nb_p_recv_Module::recv(Packet *p)
{
	hdr_cmn *ch = HDR_CMN(p);
	if(ch->direction() != hdr_cmn::UP) {
		std::cerr << "Something weird here, packet direction is not UP" << std::endl;
	} else {
		assert(recvBufLen < READ_BUF_LEN);
		recvBuf[recvBufLen] = p;
		recvBufLen++;
	}
	return;
}

void Nb_p_recv_Module::start_gen(void) {
	chkTimerPeriod.resched(120.0);
}

void Nb_p_recv_Module::stop_gen(void) {
	chkTimerPeriod.force_cancel();
	chkNetBlocksTimer.force_cancel();
}

void Nb_p_recv_Module::uwSendTimerAppl::expire(Event *e)
{
	if (isNbp_) {
		nbp__main_loop_step();
		m_->chkNetBlocksTimer.resched(10);
	} else {
		m_->sendPkt(); //TODO: This packet/video sending should be seperate from the 
		m_->chkTimerPeriod.resched(120.0); // schedule next transmission
	}
}
static int uidcnt_ = 0;
void Nb_p_recv_Module::sendPkt(void) {
	nbp__send(conn, "Client says hello", sizeof("Client says hello"));
}

double Nb_p_recv_Module::getSentPkts(void) {
	return 0.0;
}
double Nb_p_recv_Module::getRecvPkts(void) {
	return 0.0;
}
double Nb_p_recv_Module::getDropPkts(void) {
	return 0.0;
}
double Nb_p_recv_Module::getDelay(void) {
	return 0.0;
}
double Nb_p_recv_Module::getThroughput(void) {
	return 0.0;
}
double Nb_p_recv_Module::getHeaderSize(void) {
	return 0.0;
}