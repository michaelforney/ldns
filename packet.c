/*
 * packet.c
 *
 * dns packet implementation
 *
 * a Net::DNS like library for C
 *
 * (c) NLnet Labs, 2004
 *
 * See the file LICENSE for the license
 */

#include <config.h>

#include <ldns/packet.h>
#include "util.h"

/* Access functions 
 * do this as functions to get type checking
 */


/* read */
uint16_t
ldns_pkt_id(ldns_pkt *packet)
{
	return packet->_header->_id;
}

bool
ldns_pkt_qr(ldns_pkt *packet)
{
	return packet->_header->_qr;
}

bool
ldns_pkt_aa(ldns_pkt *packet)
{
	return packet->_header->_aa;
}

bool
ldns_pkt_tc(ldns_pkt *packet)
{
	return packet->_header->_tc;
}

bool
ldns_pkt_rd(ldns_pkt *packet)
{
	return packet->_header->_rd;
}

bool
ldns_pkt_cd(ldns_pkt *packet)
{
	return packet->_header->_cd;
}

bool
ldns_pkt_ra(ldns_pkt *packet)
{
	return packet->_header->_ra;
}

bool
ldns_pkt_ad(ldns_pkt *packet)
{
	return packet->_header->_ad;
}

uint8_t
ldns_pkt_opcode(ldns_pkt *packet)
{
	return packet->_header->_opcode;
}

uint8_t
ldns_pkt_rcode(ldns_pkt *packet)
{
	return packet->_header->_rcode;
}

uint16_t
ldns_pkt_qdcount(ldns_pkt *packet)
{
	return packet->_header->_qdcount;
}

uint16_t
ldns_pkt_ancount(ldns_pkt *packet)
{
	return packet->_header->_ancount;
}

uint16_t
ldns_pkt_nscount(ldns_pkt *packet)
{
	return packet->_header->_nscount;
}

uint16_t
ldns_pkt_arcount(ldns_pkt *packet)
{
	return packet->_header->_arcount;
}

ldns_rr_list *
ldns_pkt_question(ldns_pkt *packet)
{
	return packet->_question;
}

ldns_rr_list *
ldns_pkt_answer(ldns_pkt *packet)
{
	return packet->_answer;
}

ldns_rr_list *
ldns_pkt_authority(ldns_pkt *packet)
{
	return packet->_authority;
}

ldns_rr_list *
ldns_pkt_additional(ldns_pkt *packet)
{
	return packet->_additional;
}

uint16_t
ldns_pkt_xxcount(ldns_pkt *packet, ldns_pkt_section s)
{
	switch(s) {
		case LDNS_SECTION_QUESTION:
			return ldns_pkt_qdcount(packet);
		case LDNS_SECTION_ANSWER:
			return ldns_pkt_ancount(packet);
		case LDNS_SECTION_AUTHORITY:
			return ldns_pkt_nscount(packet);
		case LDNS_SECTION_ADDITIONAL:
			return ldns_pkt_arcount(packet);
	}
	return 0;
}

ldns_rr_list *
ldns_pkt_xxsection(ldns_pkt *packet, ldns_pkt_section s)
{
	switch(s) {
		case LDNS_SECTION_QUESTION:
			return ldns_pkt_question(packet);
		case LDNS_SECTION_ANSWER:
			return ldns_pkt_answer(packet);
		case LDNS_SECTION_AUTHORITY:
			return ldns_pkt_authority(packet);
		case LDNS_SECTION_ADDITIONAL:
			return ldns_pkt_additional(packet);
	}
	return NULL;
}

/* write */
void
ldns_pkt_set_id(ldns_pkt *packet, uint16_t id)
{
	packet->_header->_id = id;
}

void
ldns_pkt_set_qr(ldns_pkt *packet, bool qr)
{
	packet->_header->_qr = qr;
}

void
ldns_pkt_set_aa(ldns_pkt *packet, bool aa)
{
	packet->_header->_aa = aa;
}

void
ldns_pkt_set_tc(ldns_pkt *packet, bool tc)
{
	packet->_header->_tc = tc;
}

void
ldns_pkt_set_rd(ldns_pkt *packet, bool rd)
{
	packet->_header->_rd = rd;
}

void
ldns_pkt_set_cd(ldns_pkt *packet, bool cd)
{
	packet->_header->_cd = cd;
}

void
ldns_pkt_set_ra(ldns_pkt *packet, bool ra)
{
	packet->_header->_ra = ra;
}

void
ldns_pkt_set_ad(ldns_pkt *packet, bool ad)
{
	packet->_header->_ad = ad;
}

void
ldns_pkt_set_opcode(ldns_pkt *packet, uint8_t opcode)
{
	packet->_header->_opcode = opcode;
}

void
ldns_pkt_set_rcode(ldns_pkt *packet, uint8_t rcode)
{
	packet->_header->_rcode = rcode;
}

void
ldns_pkt_set_qdcount(ldns_pkt *packet, uint16_t qdcount)
{
	packet->_header->_qdcount = qdcount;
}

void
ldns_pkt_set_ancount(ldns_pkt *packet, uint16_t ancount)
{
	packet->_header->_ancount = ancount;
}

void
ldns_pkt_set_nscount(ldns_pkt *packet, uint16_t nscount)
{
	packet->_header->_nscount = nscount;
}

void
ldns_pkt_set_arcount(ldns_pkt *packet, uint16_t arcount)
{
	packet->_header->_arcount = arcount;
}

/** 
 * push an rr on a packet
 * \param[in] packet packet to operatore on
 * \param[in] section where to put it
 * \param[in] rr rr to push
 * \return ldns_status status
 */
bool
ldns_pkt_push_rr(ldns_pkt *packet, ldns_pkt_section section, ldns_rr *rr)
{
	ldns_rr_list *rrs;

	/* get the right rr list for this section */
	rrs = ldns_pkt_xxsection(packet, section);
	if (!rrs) {
		return false;
	}
	/* push the rr */
	ldns_rr_list_push_rr(rrs, rr);
	
	/* TODO: move this to separate function? */
	switch(section) {
		case LDNS_SECTION_QUESTION:
			ldns_pkt_set_qdcount(packet, ldns_pkt_qdcount(packet) + 1);
			break;
		case LDNS_SECTION_ANSWER:
			ldns_pkt_set_ancount(packet, ldns_pkt_ancount(packet) + 1);
			break;
		case LDNS_SECTION_AUTHORITY:
			ldns_pkt_set_nscount(packet, ldns_pkt_nscount(packet) + 1);
			break;
		case LDNS_SECTION_ADDITIONAL:
			ldns_pkt_set_arcount(packet, ldns_pkt_arcount(packet) + 1);
			break;
	}
	return true;
}


/* Create/destroy/convert functions
 */
ldns_pkt *
ldns_pkt_new()
{
	ldns_pkt *packet;
	packet = MALLOC(ldns_pkt);
	if (!packet) {
		return NULL;
	}

	packet->_header = MALLOC(ldns_hdr);
	if (!packet->_header) {
		FREE(packet);
		return NULL;
	}

	packet->_question = ldns_rr_list_new();
	packet->_answer = ldns_rr_list_new();
	packet->_authority = ldns_rr_list_new();
	packet->_additional = ldns_rr_list_new();
	return packet;
}

void
ldns_pkt_free(ldns_pkt *packet)
{
	FREE(packet->_header);
	if (packet->_question) {
		/*ldns_rr_list_destroy(packet->_question);*/
	}
	if (packet->_answer) {
		/*ldns_rr_list_destroy(packet->_answer);*/
		FREE(packet->_answer);
	}
	if (packet->_authority) {
		/*ldns_rr_list_destroy(packet->_authority);*/
		FREE(packet->_authority);
	}
	if (packet->_additional) {
		/*ldns_rr_list_destroy(packet->_additional);*/
		FREE(packet->_authority);
	}
	FREE(packet);
}

ldns_pkt *
ldns_pkt_query_new(char *name, ldns_rr_type rr_type, ldns_rr_class rr_class)
{
	ldns_pkt *packet = ldns_pkt_new();
	ldns_rr *question_rr = ldns_rr_new();
	ldns_rdf *name_rdf;

	if (rr_type == 0) {
		rr_type = LDNS_RR_TYPE_A;
	}
	if (rr_class == 0) {
		rr_class = LDNS_RR_CLASS_IN;
	}

	if (ldns_str2rdf_dname(&name_rdf, name) == LDNS_STATUS_OK) {
		ldns_rr_set_owner(question_rr, name_rdf);
		ldns_rr_set_type(question_rr, rr_type);
		ldns_rr_set_class(question_rr, rr_class);
		
		ldns_pkt_push_rr(packet, LDNS_SECTION_QUESTION, question_rr);
	} else {
		ldns_rr_free(question_rr);
		ldns_pkt_free(packet);
		return NULL;
	}
	
	return packet;
}
