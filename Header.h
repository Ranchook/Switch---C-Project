#pragma once
#include <stdio.h>
#include <stdbool.h>

#define OUTP1 "port1.out"
#define OUTP2 "port2.out"
#define OUTP3 "port3.out"
#define OUTP4 "port4.out"

typedef struct packet {
	unsigned int time;
	unsigned char Da;
	char Sa;
	char Prio;
	char Data_length;
	unsigned char* data;
	unsigned char Checksum;
} packet;

typedef struct route_node {
	unsigned char da;
	char output_port;
	struct route_node *left;
	struct route_node *right;
} S_node;

typedef struct {
	S_node *root;
}BST;

typedef struct pkt_node {
	packet  *pkt;
	struct pkt_node *next;
} S_pkt;

typedef struct Out_Qs_mgr {
	struct pkt_node  *head_p1;
	struct pkt_node  *tail_p1;

	struct pkt_node *head_p0;
	struct pkt_node *tail_p0;
} S_Out_Qs_mgr;

typedef enum Bool {
	FALSE, TRUE
}Bool;

//functions for packets
void packet_read(FILE *fp, packet *pkt);
void packet_write(FILE *fp, const packet *pkt);
bool checksum_check(const packet *pkt);

//functions for bst
S_node* create_node(unsigned char da, char Pout);
S_node *add_route(S_node *root, char da, char output_port);
void addItemToTreeRec(S_node *root, unsigned char da, char output_port);
S_node* LowestDa(S_node* node);
S_node *delete_route(S_node *root, char da);
S_node *search_route(const S_node *root, char da);
void print_routing_table(const S_node  *root);
S_node *build_route_table(FILE *fp, S_node *root);
void free_tree(S_node * node);

//functions for lists
void enque_pkt(S_Out_Qs_mgr *QM_ptr, packet *pkt);
packet *deque_pkt(S_Out_Qs_mgr *QM_ptr, char priority);
void sort_list(S_pkt* head);
S_Out_Qs_mgr* add_last(S_pkt* packet_node, S_Out_Qs_mgr* List);