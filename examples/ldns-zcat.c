/*
 * read a zone from disk and split it up:
 *
 * zone: SOA a b c d e f g h i j k l 
 * becomes:
 * zone1: SOA a b c d e f
 * zone2: SOA f g h i k l
 *
 * ldns-catzone removes the last name and put
 * the zone back together.
 *
 * This way you can incremental sign a zone
 *
 * See the file LICENSE for the license
 */

#include "config.h"
#include <errno.h>
#include <ldns/dns.h>

#define FIRST_ZONE 	0
#define MIDDLE_ZONE 	1
#define LAST_ZONE 	2

void
usage(FILE *f, char *progname)
{
		fprintf(f, "Usage: %s [OPTIONS] <zonefiles>\n", progname);
		fprintf(f, "  Concatenate signed zone snippets created with ldns-zsplit\n");
		fprintf(f, "  back together. The generate zone file is printed to stdout\n");
		fprintf(f, "  The new zone should be equal to the original zone (before splitting)\n");
		fprintf(f, "OPTIONS:\n");
		fprintf(f, "-o ORIGIN\tUse this as initial origin, for zones starting with @\n");
}

int
main(int argc, char **argv)
{
	char *progname;
	FILE *fp;
	int c;
	ldns_rdf *origin;
	size_t i, j;
	int where;
	ldns_zone *z;
	ldns_rr_list *zrr;
	ldns_rr *current_rr;
	ldns_rr *soa;
	ldns_rdf *last_owner;
	ldns_rr  *last_rr;
	ldns_rr  *pop_rr;

	progname = strdup(argv[0]);
	origin = NULL;
	
	while ((c = getopt(argc, argv, "n:o:")) != -1) {
		switch(c) {
			case 'o':
				origin = ldns_dname_new_frm_str(strdup(optarg));
				if (!origin) {
					fprintf(stderr, "Cannot convert the origin %s to a domainname\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			default:
				fprintf(stderr, "Unrecognized option\n");
				usage(stdout, progname);
				exit(EXIT_FAILURE);
		}
	}
	
	argc -= optind;
	argv += optind;

	if (argc < 1) {
		usage(stdout, progname);
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < (size_t)argc; i++) {

		if (!(fp = fopen(argv[i], "r"))) {
			fprintf(stderr, "Error opening key file %s: %s\n", argv[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		if (!(z = ldns_zone_new_frm_fp(fp, origin, 0, 0))) {
			fprintf(stderr, "Zone file %s could not be parsed correctly\n", argv[i]);
			exit(EXIT_FAILURE);
		}

		zrr = ldns_zone_rrs(z);
		soa = ldns_zone_soa(z); /* SOA is stored seperately */

		fprintf(stderr, "%s\n", argv[i]);

		if (0 == i) {
			where = FIRST_ZONE;

			/* remove the last equal named RRs */
			last_rr = ldns_rr_list_pop_rr(zrr);
			last_owner = ldns_rr_owner(last_rr);
			/* remove until no match */
			do {
				pop_rr = ldns_rr_list_pop_rr(zrr);
			} while(ldns_rdf_compare(last_owner, ldns_rr_owner(pop_rr)) == 0) ;
			/* we popped one to many, put it back */
			ldns_rr_list_push_rr(zrr, pop_rr);
		} else if ((size_t)(argc - 1) == i) {
			where = LAST_ZONE;
		} else {
			where = MIDDLE_ZONE;

			/* remove the last equal named RRs */
			last_rr = ldns_rr_list_pop_rr(zrr);
			last_owner = ldns_rr_owner(last_rr);
			/* remove until no match */
			do {
				pop_rr = ldns_rr_list_pop_rr(zrr);
			} while(ldns_rdf_compare(last_owner, ldns_rr_owner(pop_rr)) == 0) ;
			/* we popped one to many, put it back */
			ldns_rr_list_push_rr(zrr, pop_rr);
		}

		/* printing the RRs */
		for (j = 0; j < ldns_rr_list_rr_count(zrr); j++) {

			current_rr = ldns_rr_list_rr(zrr, j);
		
			switch(where) {
				case FIRST_ZONE:
					if (soa) {
						ldns_rr_print(stdout, soa);
						soa = NULL;
					}
					break;
				case MIDDLE_ZONE:
					/* rm SOA */
					/* SOA isn't printed by default */

					/* rm SOA aux records 
					 * this also takes care of the DNSKEYs + RRSIGS
					 */
					if (ldns_rdf_compare(ldns_rr_owner(current_rr),
							ldns_rr_owner(soa)) == 0) {	
						continue;
					}
					break;
				case LAST_ZONE:
					/* rm SOA */
					/* SOA isn't printed by default */

					/* rm SOA aux records 
					 * this also takes care of the DNSKEYs + RRSIGS
					 */
					if (ldns_rdf_compare(ldns_rr_owner(current_rr),
							ldns_rr_owner(soa)) == 0) {	
						continue;
					}
					break;
			}
			ldns_rr_print(stdout, current_rr);
		}
	}
        exit(EXIT_SUCCESS);
}