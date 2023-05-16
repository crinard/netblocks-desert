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
#include <fstream>
#include <string>

#include "nb_p_recv.h"
#include "nbp_runtime.h"
#define TELEM_FILE_PATH "/home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/example_data/navlog.txt" 
#define VIDEO_FILE_PATH "/home/crinard/Desktop/sample_video.mp4"

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

static int recvd_packets = 0;
static size_t recvd_bytes = 0;
static int sent_packets = 0;
static int sent_bytes = 0;
static int dropped_packets = 0;


static void callback(int event, nbp__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[1000];
		int len = nbp__read(c, buff, 1000);
		buff[len] = 0;	
		recvd_packets++;
		recvd_bytes += len;
		fprintf(stderr, "Client received %s\n", buff);
	}
}

Nb_p_recv_Module::Nb_p_recv_Module():chkTimerPeriod(this, false), chkNetBlocksTimer(this, true) {
	nbp__desert_init((void*)this);
	nbp__net_init();
	char server_id[] = {0, 0, 0, 0, 0, 1};
	char client_id[] = {0, 0, 0, 0, 0, 2};
	memcpy(nbp__my_host_id, client_id, 6);
	conn = nbp__establish(server_id, 8081, 8080, callback);
	chkNetBlocksTimer.resched(60.0);
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
		} else if (strcasecmp(argv[1], "getrecvbytes") == 0) {
			tcl.resultf("%f", getRecvBytes());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getsentbytes") == 0) {
			tcl.resultf("%f", getSentBytes());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "settelem") == 0) {
			// if (set_mode_telem()) return TCL_OK;
			return TCL_ERROR;
		} else if (strcasecmp(argv[1], "setvideostream") == 0) {
			// if (set_mode_video()) return TCL_OK;
			return TCL_ERROR;
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
		if (recvBufLen >= READ_BUF_LEN) {
			std::cerr << "Buffer overflow, dropping packet\n";
			dropped_packets++;
			drop(p, 1, "Buffer overflow");
			return;
		}
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
	nbp__desert_deinit();
}

static int uidcnt__ = 0;
void Nb_p_recv_Module::uwSendTimerAppl::expire(Event *e)
{
	if (isNbp_) {
		nbp__main_loop_step();
		m_->chkNetBlocksTimer.resched(10.0);
	} else {
		m_->sendPkt();
		m_->chkTimerPeriod.resched(120.0); // schedule next transmission
	}
}

char* Nb_p_recv_Module::genVideoPkt(int * size) {
	std::string lines[1000];
	std::ifstream myfile(VIDEO_FILE_PATH);
	int a = 0;
	if(!myfile) {
		std::cerr << "VIDEO FILE NOT FOUND\n";
		return NULL;
	}
	if (!myfile.eof()) {
		getline(myfile, lines[a],'\n');
		a++;
	}
	char* buf = (char*)malloc(lines[a].size());
	memcpy(buf, lines[a].c_str(), lines[a].size());		
	*size = lines[a].size();		
	return buf;
}

char* Nb_p_recv_Module::genTelemPkt(int * size) {
	std::ifstream fin(TELEM_FILE_PATH);
	std::string tmp;
	if(!fin) {
		std::cerr << "TELEM FILE NOT FOUND\n";
		return NULL;
	}
	for (int i = 0; !fin.eof(); i++) {
		getline(fin, tmp, '\n');
	}
	char* buf = (char*)malloc(tmp.size());
	memcpy(buf, tmp.c_str(), tmp.size());		
	*size = tmp.size();
	fin.close();
	return buf;
}

void Nb_p_recv_Module::sendPkt(void) {
	// fprintf(stdout, "sendPkt()\n");
	nbp__send(conn, "Client says hello", sizeof("Client says hello"));
	sent_packets++;
	sent_bytes+=sizeof("Client says hello");
}

double Nb_p_recv_Module::getSentPkts(void) {
	return sent_packets;
}
double Nb_p_recv_Module::getRecvPkts(void) {
	return recvd_packets;
}
double Nb_p_recv_Module::getDropPkts(void) {
	return dropped_packets;
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
double Nb_p_recv_Module::getRecvBytes(void) {
	return recvd_bytes;
}
double Nb_p_recv_Module::getSentBytes(void) {
	return sent_bytes;
}

bool Nb_p_recv_Module::set_mode_telem(void) {
	if (sim_type != NOT_SET)
		return false;
	sim_type = CONTROL_STREAM;
	return true;
}
bool Nb_p_recv_Module::set_mode_video(void) {
	if (sim_type != NOT_SET)
		return false;
	sim_type = VIDEO_STREAM;
	return true;
}