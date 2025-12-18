/* $Id$ */
/* MiniUPnP project
 * No-op redirect functions for external script mode
 * These are stub implementations that should never be called
 * when USE_EXTERNAL_SCRIPT is active */

#ifdef USE_EXTERNAL_SCRIPT

#include <syslog.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "../config.h"
#include "rdr_desc.h"

/* Stub functions - these should not be called when using external script */

void set_rdr_name(const char * name)
{
	(void)name;
	syslog(LOG_ERR, "set_rdr_name() called but should not be used with external script");
}

int init_redirect(void)
{
	/* No initialization needed for external script mode */
	return 0;
}

void shutdown_redirect(void)
{
	/* No cleanup needed for external script mode */
}

int add_redirect_rule2(const char * ifname,
                      const char * rhost, unsigned short eport,
                      const char * iaddr, unsigned short iport, int proto,
                      const char * desc, unsigned int timestamp)
{
	(void)ifname; (void)rhost; (void)eport; (void)iaddr;
	(void)iport; (void)proto; (void)desc; (void)timestamp;
	syslog(LOG_ERR, "add_redirect_rule2() called but should not be used with external script");
	return -1;
}

int add_filter_rule2(const char * ifname,
                    const char * rhost, const char * iaddr,
                    unsigned short eport, unsigned short iport,
                    int proto, const char * desc)
{
	(void)ifname; (void)rhost; (void)iaddr; (void)eport;
	(void)iport; (void)proto; (void)desc;
	syslog(LOG_ERR, "add_filter_rule2() called but should not be used with external script");
	return -1;
}

int get_redirect_rule(const char * ifname, unsigned short eport, int proto,
                     char * iaddr, int iaddrlen, unsigned short * iport,
                     char * desc, int desclen,
                     char * rhost, int rhostlen,
                     unsigned int * timestamp,
                     u_int64_t * packets, u_int64_t * bytes)
{
	(void)ifname; (void)rhost; (void)rhostlen; (void)packets; (void)bytes;
	/* Use internal tracking list */
	return get_redirect_rule_from_desc_list(eport, proto,
	                                         iaddr, iaddrlen, iport,
	                                         desc, desclen, timestamp);
}

int get_redirect_rule_by_index(int index,
                               char * ifname, unsigned short * eport,
                               char * iaddr, int iaddrlen, unsigned short * iport,
                               int * proto, char * desc, int desclen,
                               char * rhost, int rhostlen,
                               unsigned int * timestamp,
                               u_int64_t * packets, u_int64_t * bytes)
{
	(void)ifname; (void)rhostlen; (void)packets; (void)bytes;
	/* Use internal tracking list */
	return get_redirect_rule_by_index_from_desc_list(index, eport, proto,
	                                                  iaddr, iaddrlen, iport,
	                                                  desc, desclen, timestamp);
}

int delete_redirect_and_filter_rules(unsigned short eport, int proto)
{
	(void)eport; (void)proto;
	syslog(LOG_ERR, "delete_redirect_and_filter_rules() called but should not be used with external script");
	return -1;
}

int delete_filter_rule(const char * ifname, unsigned short port, int proto)
{
	(void)ifname; (void)port; (void)proto;
	syslog(LOG_ERR, "delete_filter_rule() called but should not be used with external script");
	return -1;
}

int update_portmapping(const char * ifname, unsigned short eport, int proto,
                      unsigned short iport, const char * desc,
                      unsigned int timestamp)
{
	(void)ifname; (void)eport; (void)proto; (void)iport;
	(void)desc; (void)timestamp;
	syslog(LOG_ERR, "update_portmapping() called but should not be used with external script");
	return -1;
}

int update_portmapping_desc_timestamp(const char * ifname,
                                      unsigned short eport, int proto,
                                      const char * desc, unsigned int timestamp)
{
	(void)ifname; (void)eport; (void)proto; (void)desc; (void)timestamp;
	syslog(LOG_ERR, "update_portmapping_desc_timestamp() called but should not be used with external script");
	return -1;
}

int get_portmappings_in_range(unsigned short startport, unsigned short endport,
                              int proto, unsigned int * number)
{
	(void)startport; (void)endport; (void)proto; (void)number;
	syslog(LOG_ERR, "get_portmappings_in_range() called but should not be used with external script");
	return -1;
}

#endif /* USE_EXTERNAL_SCRIPT */
