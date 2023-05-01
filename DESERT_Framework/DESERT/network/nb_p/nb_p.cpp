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
#include "nb_runtime.h"
// #include "net-blocks/scratch/gen_headers.h"
extern char nb__my_host_id[];

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
int running = 1;
static void callback(int event, nb__connection_t * c) {
	std::cout << "callback called\n, event = " << event << "\n";
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[65];
		int len = nb__read(c, buff, 64);
		buff[len] = 0;	
		printf("Received = %s\n", buff);	
		running = 0;
	}
}

Nb_pModule::Nb_pModule():chkTimerPeriod(this), chkNetBlocksTimer(this) {
	std::cout << "Nb_pModule::Nb_pModule()::chkTImerPeriod" << std::endl;
	nb__desert_init((void*)this);
	nb__net_init();
	char server_id[] = {0, 0, 0, 0, 0, 1};
	char client_id[] = {0, 0, 0, 0, 0, 2};
	std::cout << "server_id = " << server_id << std::endl;
	memcpy(nb__my_host_id, server_id, 6);
	std::cout << "nb__my_host_id = " << nb__my_host_id << std::endl;
	conn = nb__establish(client_id, 8081, 8080, callback);
	std::cout << "conn = " << conn << std::endl;
	chkNetBlocksTimer.resched(10.0);
	std::cout << "initialized\n";
	recvBuf = (Packet**) calloc(READ_BUF_LEN, sizeof(Packet*));
	recvBufLen = 0;
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
			std::cout << "***********start***********\n";
			start_gen();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "stop") == 0) {
			std::cout << "***********stop***********\n";
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
	if(ch->direction() != hdr_cmn::UP) {
		std::cerr << "Something weird here, packet direction is not UP" << std::endl;
	} else {
		fprintf(stderr, "this = %lu, Received packet recvBufLen = %lu, READ_BUF_LEN = %lu\n", (uint64_t) this, recvBufLen, READ_BUF_LEN);
		assert(recvBufLen < READ_BUF_LEN);
		recvBuf[recvBufLen] = p;
		recvBufLen++;
	}
	return;
}

void Nb_pModule::start_gen(void) {
	chkTimerPeriod.resched(120.0);
}

void Nb_pModule::stop_gen(void) {
	std::cout << "stop_gen\n";
	chkTimerPeriod.force_cancel();
}

void Nb_pModule::uwSendTimerAppl::expire(Event *e)
{
	m_->sendPkt(); //TODO: This packet/video sending should be seperate from the 
	m_->chkTimerPeriod.resched(120.0); // schedule next transmission
	std::cout << "uwSendTimerAppl::expire\n";
}
static int uidcnt_ = 0;
void Nb_pModule::sendPkt(void) {

	// Packet *p = Packet::alloc();
	// hdr_cmn *ch = hdr_cmn::access(p);
	// ch->uid() = uidcnt_++;
	// ch->ptype() = 2;
	// ch->size() = 125;
	nb__send(conn, "Hello World", sizeof("Hello World"));
	nb__main_loop_step();
	std::cout << "sendPkt\n";
	// sendDown(p,0);
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