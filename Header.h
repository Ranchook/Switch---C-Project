#pragma once
#include <stdio.h>
#include <stdbool.h>

// Output file macros
#define OUTP1 "port1.out"
#define OUTP2 "port2.out"
#define OUTP3 "port3.out"
#define OUTP4 "port4.out"

// Packet structure definition
typedef struct packet {
    unsigned int time;
    unsigned char Da;        // Destination address
    char Sa;                 // Source address
    char Prio;               // Priority
    char Data_length;
    unsigned char* data;
    unsigned char Checksum;
} packet;

// Binary Search Tree node structure
typedef struct route_node {
    unsigned char da;        // Destination address
    char output_port;
    struct route_node *left;
    struct route_node *right;
} S_node;

// Binary Search Tree structure
typedef struct {
    S_node *root;
}BST;

// Linked list node for packets
typedef struct pkt_node {
    packet  *pkt;
    struct pkt_node *next;
} S_pkt;

// Output queue manager structure
typedef struct Out_Qs_mgr {
    struct pkt_node  *head_p1;
    struct pkt_node  *tail_p1;
    struct pkt_node *head_p0;
    struct pkt_node *tail_p0;
} S_Out_Qs_mgr;

// Boolean enum
typedef enum Bool {
    FALSE, TRUE
}Bool;

// Function prototypes for packet operations
void packet_read(FILE *fp, packet *pkt);
void packet_write(FILE *fp, const packet *pkt);
bool checksum_check(const packet *pkt);

// Function prototypes for BST operations
S_node* create_node(unsigned char da, char Pout);
S_node *add_route(S_node *root, char da, char output_port);
void addItemToTreeRec(S_node *root, unsigned char da, char output_port);
S_node* LowestDa(S_node* node);
S_node *delete_route(S_node *root, char da);
S_node *search_route(const S_node *root, char da);
void print_routing_table(const S_node  *root);
S_node *build_route_table(FILE *fp, S_node *root);
void free_tree(S_node * node);

// Function prototypes for list operations
void enque_pkt(S_Out_Qs_mgr *QM_ptr, packet *pkt);
packet *deque_pkt(S_Out_Qs_mgr *QM_ptr, char priority);
void sort_list(S_pkt* head);
S_Out_Qs_mgr* add_last(S_pkt* packet_node, S_Out_Qs_mgr* List);
