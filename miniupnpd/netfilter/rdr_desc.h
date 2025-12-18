/* $Id$ */
/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * MiniUPnP project
 * Redirect description tracking for UPnP queries
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef RDR_DESC_H_INCLUDED
#define RDR_DESC_H_INCLUDED

/* Functions for internal port mapping tracking (used by UPnP queries) */
void
add_redirect_desc(unsigned short eport, int proto,
                  const char * iaddr, unsigned short iport,
                  const char * desc, unsigned int timestamp);

void
del_redirect_desc(unsigned short eport, int proto);

void
get_redirect_desc(unsigned short eport, int proto,
                  char * desc, int desclen,
                  unsigned int * timestamp);

int
get_redirect_rule_from_desc_list(unsigned short eport, int proto,
                                   char * iaddr, int iaddrlen,
                                   unsigned short * iport,
                                   char * desc, int desclen,
                                   unsigned int * timestamp);

int
get_redirect_rule_by_index_from_desc_list(int index,
                                           unsigned short * eport,
                                           int * proto,
                                           char * iaddr, int iaddrlen,
                                           unsigned short * iport,
                                           char * desc, int desclen,
                                           unsigned int * timestamp);

#endif /* RDR_DESC_H_INCLUDED */
