/**
 * @file   nb_p.cpp
 * @version 1.0.0
 *
 * \brief Provides the class implementation of NB_P.
 *
 * Provides the class implementation of NB_P.
 */

#include "nb_p.h"

#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>

#include "nb_runtime.h"
#define MAX_TX_LEN 980
#define TELEM_FILE_PATH                                                    \
  "/home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/example_data/" \
  "navlog.txt"
#define VIDEO_FILE_PATH \
  "/home/crinard/Desktop/sample_video.mp4"  // TODO: Fix this
extern char nb1::nb__my_host_id[];
extern char nb2::nb__my_host_id[];

static int num_instances = 0;
/**
 * Adds the module for Nb_pModuleClass in ns2.
 */
static class Nb_pModuleClass : public TclClass {
 public:
  Nb_pModuleClass() : TclClass("Module/UW/Nb_p") {}

  TclObject *create(int, const char *const *) { return (new Nb_pModule); }
} class_nb_p_module;

static int recvd_packets = 0;
static size_t recvd_bytes = 0;
static int sent_packets = 0;
static int sent_bytes = 0;

static void callbackNB1(int event, nb1::nb__connection_t *c) {
  if (event == QUEUE_EVENT_READ_READY) {
    char buff[1000];
    int len = nb1::nb__read(c, buff, 1000);
    buff[len] = 0;
    printf("Nb1 recieved %s from Nb2\n", buff);
    recvd_packets++;
    recvd_bytes += len;
  }
}
static void callbackNB2(int event, nb2::nb__connection_t *c) {
  if (event == QUEUE_EVENT_READ_READY) {
    char buff[1000];
    int len = nb2::nb__read(c, buff, 1000);
    buff[len] = 0;
    printf("Nb2 recieved %s from Nb1\n", buff);
    recvd_packets++;
    recvd_bytes += len;
  }
}
Nb_pModule::Nb_pModule()
    : chkTimerPeriod(this, false),
      chkNetBlocksTimer(this, true),
      period_(60.0) {
  instance_num = num_instances;
  num_instances++;
  if (instance_num == 0) {
    nb1::nb__desert_init((void *)this);
    nb1::nb__net_init();
    char server_id[] = {0, 0, 0, 0, 0, 1};
    char client_id[] = {0, 0, 0, 0, 0, 2};
    memcpy(nb1::nb__my_host_id, server_id, 6);
    conn = (void *)nb1::nb__establish(client_id, 8080, 8081, callbackNB1);
    chkNetBlocksTimer.resched(10.0);
    recvBuf = (Packet **)calloc(READ_BUF_LEN, sizeof(Packet *));
    recvBufLen = 0;
    sim_type = NOT_SET;
    bind("period_", &period_);
  } else if (instance_num == 1) {
    nb2::nb__desert_init((void *)this);
    nb2::nb__net_init();
    char server_id[] = {0, 0, 0, 0, 0, 1};
    char client_id[] = {0, 0, 0, 0, 0, 2};
    memcpy(nb2::nb__my_host_id, client_id, 6);
    conn = (void *)nb2::nb__establish(server_id, 8081, 8080, callbackNB2);
    chkNetBlocksTimer.resched(10.0);
    recvBuf = (Packet **)calloc(READ_BUF_LEN, sizeof(Packet *));
    recvBufLen = 0;
    sim_type = NOT_SET;
    bind("period_", &period_);
  } else {
    std::cerr << "Error: Nb_pModule only supports 2 instances ln95\n";
    assert(false);
  }
}

Nb_pModule::~Nb_pModule() {
  if (instance_num == 0) {
    nb1::nb__desert_deinit();
  } else if (instance_num == 1) {
    nb2::nb__desert_deinit();
  } else {
    std::cerr << "Error: Nb_pModule only supports 2 instances ln106\n";
    assert(false);
  }
  chkNetBlocksTimer.force_cancel();
  chkTimerPeriod.force_cancel();
}

int Nb_pModule::recvSyncClMsg(ClMessage *m) { return Module::recvSyncClMsg(m); }

int Nb_pModule::command(int argc, const char *const *argv) {
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
      if (set_mode_telem()) return TCL_OK;
      return TCL_ERROR;
    }
  }
  return Module::command(argc, argv);
}

void Nb_pModule::recv(Packet *p) {
  hdr_cmn *ch = HDR_CMN(p);
  if (ch->direction() != hdr_cmn::UP) {
    std::cerr << "Something weird here, packet direction is not UP"
              << std::endl;
  } else {
    assert(recvBufLen < READ_BUF_LEN);
    recvBuf[recvBufLen] = p;
    recvBufLen++;
  }
  return;
}

void Nb_pModule::start_gen(void) { chkTimerPeriod.resched(period_); }

void Nb_pModule::stop_gen(void) {
  std::cout << "stop_gen\n";
  chkTimerPeriod.force_cancel();
  chkNetBlocksTimer.force_cancel();
  if (instance_num == 0)
    nb1::nb__desert_deinit();
  else if (instance_num == 1)
    nb2::nb__desert_deinit();
}

void Nb_pModule::uwSendTimerAppl::expire(Event *e) {
  if (isNb_) {
    m_->chkNetBlocksTimer.resched(10.0);
  } else {
    m_->sendPkt();
    m_->chkTimerPeriod.resched(m_->period_);  // schedule next transmission
  }
  if (m_->instance_num == 0) {
    std::cout << "NB1 period: " << m_->period_ << "\n";
    nb1::nb__main_loop_step();
  } else if (m_->instance_num == 1) {
    std::cout << "NB2 period: " << m_->period_ << "\n";
    nb2::nb__main_loop_step();
  } else
    assert(false);
}

void Nb_pModule::sendPkt(void) {
  if (sim_type == NOT_SET) {
    std::cerr << "sim_type not set\n";
    if (instance_num == 0)
      nb1::nb__send((nb1::nb__connection_t *)conn, "Hello", sizeof("Hello"));
    // if (instance_num == 1)
    //   nb2::nb__send((nb2::nb__connection_t *)conn, "Hello", sizeof("Hello"));
    // else
    //   assert(false);
    sent_packets++;
    sent_bytes += sizeof("Hello");
    return;
  } else if (sim_type == CONTROL_STREAM) {
    int r = rand() % 100;
    if (instance_num == 0)
      nb1::nb__send((nb1::nb__connection_t *)conn, CONTROL_MSG,
                    sizeof(CONTROL_MSG) - r);
    // if (instance_num == 1)
    //   nb2::nb__send((nb2::nb__connection_t *)conn, CONTROL_MSG,
    //                 sizeof(CONTROL_MSG) - r);
    // else
    //     // assert(false);
    sent_packets++;
    sent_bytes += sizeof(CONTROL_MSG) - r;
  }
}

double Nb_pModule::getSentPkts(void) { return sent_packets; }
double Nb_pModule::getRecvPkts(void) { return recvd_packets; }

double Nb_pModule::getDropPkts(void) { return 0.0; }
double Nb_pModule::getDelay(void) { return 0.0; }
double Nb_pModule::getRecvBytes(void) { return recvd_bytes; }
double Nb_pModule::getHeaderSize(void) { return 0.0; }
bool Nb_pModule::set_mode_telem(void) {
  if (sim_type != NOT_SET) return false;
  sim_type = CONTROL_STREAM;
  return true;
}

double Nb_pModule::getSentBytes(void) { return sent_bytes; }