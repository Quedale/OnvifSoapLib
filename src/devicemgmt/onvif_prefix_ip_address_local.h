#ifndef ONVIF_PREFIX_IP_ADDRESS_LOCAL_H_ 
#define ONVIF_PREFIX_IP_ADDRESS_LOCAL_H_

#include "onvif_prefix_ip_address.h"
#ifdef SOAP_H_FILE      /* if set, use the soapcpp2-generated fileH.h file as specified with: cc ... -DSOAP_H_FILE=fileH.h */
# include "stdsoap2.h"
# include SOAP_XSTRINGIFY(SOAP_H_FILE)
#else
# include "generated/soapH.h"	/* or manually replace with soapcpp2-generated *H.h file */
#endif

OnvifPrefixedIPAddress * 
OnvifPrefixedIPAddress__create4(struct tt__PrefixedIPv4Address * addr);

OnvifPrefixedIPAddress * 
OnvifPrefixedIPAddress__create6(struct tt__PrefixedIPv6Address * addr);

#endif
