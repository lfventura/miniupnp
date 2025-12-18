/* $Id$ */
/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * MiniUPnP project
 * Redirect description tracking for UPnP queries
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "../config.h"
#include "rdr_desc.h"

/* rdr_desc structure for internal tracking */
struct rdr_desc {
	struct rdr_desc * next;
	unsigned int timestamp;
	unsigned short eport;
	unsigned short iport;
	short proto;
	char iaddr[16];  /* Internal IP address */
	char str[];      /* Description */
};

static struct rdr_desc * rdr_desc_list = 0;

/* add_redirect_desc()
 * add a description to the list of redirection descriptions */
void
add_redirect_desc(unsigned short eport, int proto,
                  const char * iaddr, unsigned short iport,
                  const char * desc, unsigned int timestamp)
{
	struct rdr_desc * p;
	size_t l;
	/* set a default description if none given */
	if(!desc)
		desc = "miniupnpd";
	l = strlen(desc) + 1;
	p = malloc(sizeof(struct rdr_desc) + l);
	if(p)
	{
		p->next = rdr_desc_list;
		p->timestamp = timestamp;
		p->eport = eport;
		p->iport = iport;
		p->proto = (short)proto;
		strncpy(p->iaddr, iaddr ? iaddr : "", sizeof(p->iaddr));
		memcpy(p->str, desc, l);
		rdr_desc_list = p;
	}
}

/* del_redirect_desc()
 * delete a description from the list */
void
del_redirect_desc(unsigned short eport, int proto)
{
	struct rdr_desc * p, * last;
	p = rdr_desc_list;
	last = 0;
	while(p)
	{
		if(p->eport == eport && p->proto == proto)
		{
			if(!last)
				rdr_desc_list = p->next;
			else
				last->next = p->next;
			free(p);
			return;
		}
		last = p;
		p = p->next;
	}
}

/* get_redirect_desc()
 * go through the list to find the description */
void
get_redirect_desc(unsigned short eport, int proto,
                  char * desc, int desclen,
                  unsigned int * timestamp)
{
	struct rdr_desc * p;
	for(p = rdr_desc_list; p; p = p->next)
	{
		if(p->eport == eport && p->proto == proto)
		{
			if(desc)
				strncpy(desc, p->str, desclen);
			if(timestamp)
				*timestamp = p->timestamp;
			return;
		}
	}
}

/* get_redirect_rule_from_desc_list()
 * Get full redirect rule info from internal tracking list
 * Returns 0 on success, -1 if not found */
int
get_redirect_rule_from_desc_list(unsigned short eport, int proto,
                                   char * iaddr, int iaddrlen,
                                   unsigned short * iport,
                                   char * desc, int desclen,
                                   unsigned int * timestamp)
{
	struct rdr_desc * p;
	for(p = rdr_desc_list; p; p = p->next)
	{
		if(p->eport == eport && p->proto == proto)
		{
			if(iaddr && iaddrlen > 0)
				strncpy(iaddr, p->iaddr, iaddrlen);
			if(iport)
				*iport = p->iport;
			if(desc && desclen > 0)
				strncpy(desc, p->str, desclen);
			if(timestamp)
				*timestamp = p->timestamp;
			return 0;
		}
	}
	return -1;
}

/* get_redirect_rule_by_index_from_desc_list()
 * Get redirect rule by index from internal tracking list
 * Returns 0 on success, -1 if not found */
int
get_redirect_rule_by_index_from_desc_list(int index,
                                           unsigned short * eport,
                                           int * proto,
                                           char * iaddr, int iaddrlen,
                                           unsigned short * iport,
                                           char * desc, int desclen,
                                           unsigned int * timestamp)
{
	struct rdr_desc * p;
	int i;
	
	for(p = rdr_desc_list, i = 0; p; p = p->next, i++)
	{
		if(i == index)
		{
			if(eport)
				*eport = p->eport;
			if(proto)
				*proto = p->proto;
			if(iaddr && iaddrlen > 0)
				strncpy(iaddr, p->iaddr, iaddrlen);
			if(iport)
				*iport = p->iport;
			if(desc && desclen > 0)
				strncpy(desc, p->str, desclen);
			if(timestamp)
				*timestamp = p->timestamp;
			return 0;
		}
	}
	return -1;
}
