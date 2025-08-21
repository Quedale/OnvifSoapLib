/*****
 * 
 * This is a very dirty udp broadcaster simply meant for troubleshooting
 * It can be used to broadcast customized messages
 * 
 * *******/

#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>

#define BROADCAST_PORT 3702
#define MAX_UDP_MSG_SIZE 65535

static void
broadcast(const char *mess)
{
    struct sockaddr_in s;
    fd_set readfd;
    int count = 1;
    char buffer[MAX_UDP_MSG_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(socklen_t);
    int broadcastSock = socket(AF_INET, SOCK_DGRAM, 0);
    if(broadcastSock < 0){
        printf("Failed to create socket.\n");
        return;
    }

    int broadcastEnable=1;
    int ret=setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if(ret == -1){
        printf("Enable to set socket options\n");
        return;
    }


    struct sockaddr_in UDPsrv, peer;
    memset(&UDPsrv,0,sizeof(UDPsrv));
    UDPsrv.sin_family = AF_INET;
    UDPsrv.sin_port   = htons(BROADCAST_PORT);
    UDPsrv.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(broadcastSock, (struct sockaddr*)&UDPsrv, sizeof(UDPsrv))) {
	perror("Can't bind UDP server");
	return;
    }

    memset(&s, 0, addr_len);
    s.sin_family = AF_INET;
    s.sin_port = htons(BROADCAST_PORT);
    s.sin_addr.s_addr = INADDR_BROADCAST; /* This is not correct : htonl(INADDR_BROADCAST); */

    // printf("broadcast %s to %d\n", mess, broadcastSock);
    
    FD_ZERO(&readfd);
    FD_SET(broadcastSock, &readfd);
    
    ret = sendto(broadcastSock, mess, strlen(mess), 0, (struct sockaddr *)&s, addr_len);
    if(ret < 0){
        printf("ERROR : sendto %d\n", ret);
        return;
    }


    printf("select\n");
    ret = select(broadcastSock + 1, &readfd, NULL, NULL, NULL);
    if (ret > 0) {
        printf("select fdisset\n");
      if (FD_ISSET(broadcastSock, &readfd)) {
            while(1){
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
        memset(&buffer, 0, MAX_UDP_MSG_SIZE);
        count = recvfrom(broadcastSock, buffer, MAX_UDP_MSG_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len);
        printf("\t %d recvmsg is %s\n", count, buffer);
            }
        // if (strstr(buffer, IP_FOUND_ACK)) {
        //   printf("\tfound server IP is %s, Port is %d\n", inet_ntoa(server_addr.sin_addr),htons(server_addr.sin_port));
        // }
      }
    }
}

int readprobefile(char * filename, char * myString){
    FILE *fptr;
    int ret = 0;
    // Open a file in read mode
    fptr = fopen(filename, "r");

    // If the file exist
    if(fptr != NULL) {
        if(!fread(myString, MAX_UDP_MSG_SIZE, 1, fptr)){
            ret = 1;
        }
    // If the file does not exist
    } else {
        printf("Not able to open the file.\n");
    }

    // Close the file
    fclose(fptr);
    return ret;
}

int main(int argc, char** argv)
{

        // Store the content of the file
    char myString[MAX_UDP_MSG_SIZE];
    if(!readprobefile("./mgrprobe.xml",myString)){
        printf("Unable to find probe message.\n");
        return 1;
    };

    broadcast(
        myString
        //OnvifMgr
        // "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:c14n=\"http://www.w3.org/2001/10/xml-exc-c14n#\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\" xmlns:saml1=\"urn:oasis:names:tc:SAML:1.0:assertion\" xmlns:saml2=\"urn:oasis:names:tc:SAML:2.0:assertion\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\" xmlns:wsc=\"http://docs.oasis-open.org/ws-sx/ws-secureconversation/200512\" xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\" xmlns:chan=\"http://schemas.microsoft.com/ws/2005/02/duplex\" xmlns:wsa5=\"http://www.w3.org/2005/08/addressing\" xmlns:wsdd=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" xmlns:xmime=\"http://tempuri.org/xmime.xsd\" xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" xmlns:wstop=\"http://docs.oasis-open.org/wsn/t-1\" xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:wsrfbf=\"http://docs.oasis-open.org/wsrf/bf-2\" xmlns:wsrfr=\"http://docs.oasis-open.org/wsrf/r-2\" xmlns:tas=\"http://www.onvif.org/ver10/advancedsecurity/wsdl\" xmlns:tdn=\"http://www.onvif.org/ver10/network/wsdl\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" xmlns:tev=\"http://www.onvif.org/ver10/events/wsdl\" xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" xmlns:timg=\"http://www.onvif.org/ver20/imaging/wsdl\" xmlns:tmd=\"http://www.onvif.org/ver10/deviceIO/wsdl\" xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\" xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\"><SOAP-ENV:Header><wsa5:MessageID>urn:uuid:923373a0-9435-4839-8759-73541ece00a5</wsa5:MessageID><wsa5:ReplyTo SOAP-ENV:mustUnderstand=\"true\"><wsa5:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</wsa5:Address></wsa5:ReplyTo><wsa5:To SOAP-ENV:mustUnderstand=\"true\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa5:To><wsa5:Action SOAP-ENV:mustUnderstand=\"true\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</wsa5:Action></SOAP-ENV:Header><SOAP-ENV:Body><wsdd:Probe><wsdd:Types>tdn:NetworkVideoTransmitter</wsdd:Types></wsdd:Probe></SOAP-ENV:Body></SOAP-ENV:Envelope>"
        //ODM
        // "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"><s:Header><a:Action s:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action><a:MessageID>uuid:7474edd8-bffa-4b55-8b2f-6f8aca448a3a</a:MessageID><a:ReplyTo><a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address></a:ReplyTo><a:To s:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To></s:Header><s:Body><Probe xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\"><d:Types xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" xmlns:dp0=\"http://www.onvif.org/ver10/network/wsdl\">dp0:NetworkVideoTransmitter</d:Types></Probe></s:Body></s:Envelope>"
    );
}