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
#include "net-blocks/runtime/nb_runtime.h"
// #include "net-blocks/scratch/gen_headers.h"
// extern char nb__my_host_id[];

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
		// std::cout << 
		return (new Nb_pModule);
	}
} class_nb_p_module;
int running = 1;
static void callback(int event, nb__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[65];
		int len = nb__read(c, buff, 64);
		buff[len] = 0;	
		printf("Received = %s\n", buff);	
		running = 0;
	}
}

Nb_pModule::Nb_pModule():chkTimerPeriod(this), chkNetBlocksTimer(this) {
	nb__desert_init((void*)this);
	nb__net_init();
	char server_id[] = {0, 0, 0, 0, 0, 1};

	// memcpy(nb__my_host_id, server_id, 6);

	conn = nb__establish(0, 8081, 8080, callback);
	chkNetBlocksTimer.resched(10.0);
}

Nb_pModule::~Nb_pModule(){
	nb__destablish(conn);
	chkNetBlocksTimer.force_cancel();
}

int Nb_pModule::recvSyncClMsg(ClMessage *m)
{
	return Module::recvSyncClMsg(m);
}

int Nb_pModule::command(int argc, const char *const *argv)
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

void Nb_pModule::recv(Packet *p)
{
	hdr_cmn *ch = HDR_CMN(p);
	std::cout << "NB_P::RECV::DIRECTION::" << ch->direction() << std::endl;
	if(ch->direction() != hdr_cmn::UP) {
		std::cerr << "Something weird here, packet direction is not UP" << std::endl;
	} else {
		// Add to netblocks buffer.
		// pktBuffer.push_back(p);
		std::cout << "recv" << std::endl;
	}
	return;
}

void Nb_pModule::start_gen(void) {
	chkTimerPeriod.resched(120.0);
}

void Nb_pModule::stop_gen(void) {
	chkTimerPeriod.force_cancel();
}

void Nb_pModule::uwSendTimerAppl::expire(Event *e)
{
	m_->sendPkt(); //TODO: This packet/video sending should be seperate from the 
	m_->chkTimerPeriod.resched(120.0); // schedule next transmission
}

// void Nb_pModule::uwNetBlocksTimer::expire(Event *e)
// {
// 	nb__main_loop_step();
// 	m_->chkNetBlocksTimer.resched(10); // schedule next transmission
// }

void Nb_pModule::sendPkt(void) {

	Packet *p = Packet::alloc();
	hdr_cmn *ch = hdr_cmn::access(p);
	// incrPktSent();
    nb__main_loop_step();
	ch->timestamp() = Scheduler::instance().clock();
	sendDown(p,0);
}

double Nb_pModule::getSentPkts(void) {
	return 0.0;
}
double Nb_pModule::getRecvPkts(void) {
	return 0.0;
}
double Nb_pModule::getDropPkts(void) {
	return 0.0;
}
double Nb_pModule::getDelay(void) {
	return 0.0;
}
double Nb_pModule::getThroughput(void) {
	return 0.0;
}
double Nb_pModule::getHeaderSize(void) {
	return 0.0;
}