
/*
 * eth0: 192.168.120.233
 * eth1: 192.168.220.235
 * 
 
iptables -t nat -A POSTROUTING -o eth1 -d 192.168.220.236 -s 192.168.220.235 -j SNAT --to-source 192.168.120.234

iptables -t nat -A PREROUTING -i eth0 -s 192.168.120.234 -d 192.168.220.236 -j DNAT --to-destination 192.168.120.233

iptables -t nat -A POSTROUTING -o eth0 -d 192.168.120.234 -s 192.168.120.233 -j SNAT --to-source 192.168.220.236

iptables -t nat -A PREROUTING -i eth1 -s 192.168.220.236 -d 192.168.120.234 -j DNAT --to-destination 192.168.220.235 
 * 
 * 
# Regeln anzeigen lassen:
iptables -t nat -L
iptables -t mangle -L
 

 # Alle Regeln in 'Kette' loeschen:
iptables -t nat -F PREROUTING
iptables -t nat -F POSTROUTING
iptables -t mangle -F PREROUTING
iptables -t mangle -F POSTROUTING
 * 
 * 
 * 
arp Tabelle anzeigen
sudo arp

arp eintrag loeschen
sudo arp -d [IP]

arp Eintrag hinzufuegen
sudo arp -s 192.168.220.236 FF:FF:FF:FF:FF:FF
sudo arp -s 192.168.120.234 FF:FF:FF:FF:FF:FF

 * Netzwerkgeschwindigkeit setzen
sudo ethtool -s eth0 speed 10 duplex full autoneg on 
sudo ethtool -s eth1 speed 10 duplex full autoneg on 
http://wiki.ubuntuusers.de/ethtool
 * 

 * Netzwerkgeschwindigkeit anzeigen:
sudo iftop
oder
sudo iptraf
 * 
 * 
 *  */

#ifndef SERVERCLIENTINFO_H
#define	SERVERCLIENTINFO_H

#include <time.h>

#define CLIENT_IP "" 
#define SERVER_IP "134.99.147.228" // IP von strongrom.norgoe.de
//#define SERVER_IP "127.0.0.1"

//#define CLIENT_IP "192.168.220.235"
//#define SERVER_IP "192.168.220.236"

//#define CLIENT_IP "192.168.2.227" // eth0
//#define CLIENT_IP "192.168.2.205" // eth1


/*
 * max. UMTS Datenrate: HSPA+ 42 MBits/Sek
 * 42 MBits/Sek = 42000000 Bits/Sek = 5250000 Bytes/Sek
 */
#define MAX_UMTS_DATA_RATE 5250000

#define START_RECV_DATA_RATE 64000 //64 kBits

/*
 * Ethernet HEADER: 26 Byte (http://wiki.wireshark.org/Ethernet)
 * - 8 Bytes Preamble
 * - 6 Byte Ziel MAC
 * - 6 Byte Absender MAC
 * - 2 Type/Length (0x8000 IP)
 * - 4 Byte CRC
 * 
 * IP Header: 20 Bytes
 *
 * UDP Header: 8 Bytes
 */
#define HEADER_SIZES 54

struct init_info_client_to_server {
    int paket_size;
    char zeit_dateiname[16];
};

struct init_info_server_to_client {
    int port;
    bool log_files_ok;
};

struct paket_header {
    int train_id;
    int retransfer_train_id;
    int paket_id;
    
    int count_pakets_in_train;
    
    int recv_data_rate; // Bytes per Sek

    int last_recv_train_id;
    int last_recv_retransfer_train_id;
    int last_recv_paket_id;
    int last_recv_paket_bytes;
    
    int timeout_time_tv_sec;
    int timeout_time_tv_usec;

    double rrt;

    struct timespec recv_time;
    struct timespec send_time;
};

struct paket {
    struct paket_header header;
    char *puffer;
};

/*
 * max. UMTS Datenrate: HSPA+ 42 MBits/Sek
 * 42 MBits/Sek = 42000000 Bits/Sek = 5250000 Bytes/Sek
 */
//static int max_umts_data_rate = 5250000; 

#endif	/* SERVERCLIENTINFO_H */

