#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Header.h"

int main(int argc, char* argv[])
{
	char* route = argv[5];
	int i;

	// Creating binary search tree using route.txt file
	BST bst;
	bst.root = NULL;
	FILE *Froute = fopen(route, "r");
	assert(Froute);
	bst.root = build_route_table(Froute, bst.root);
	fclose(Froute);

	//Opening input port files
	FILE* in_ports[4];
	for (i = 0; i < 4; i++)
	{
		in_ports[i] = fopen(argv[i + 1], "r");
		assert(in_ports[i]);
	}

	//Creating & initializing all 4 output queue managers
	S_Out_Qs_mgr *lists[4];
	for (i = 0; i < 4; i++)
	{
		lists[i] = (S_Out_Qs_mgr*)calloc(1, sizeof(S_Out_Qs_mgr));
		assert(lists[i]);
	}

	// Reading packets from each file separately and putting in designated list
	packet *pkt;
	for (i = 0; i < 4; i++)
	{
		while (!feof(in_ports[i]))
		{
			pkt = (packet*)calloc(1, sizeof(packet));
			assert(pkt);
			packet_read(in_ports[i], pkt);
			if (!pkt->data) // if end of file - free packet
				free(pkt);
			else
			{
				S_node *dest_port = search_route(bst.root, pkt->Da);
				if (dest_port)
				{
					// Enqueue packet to appropriate output queue
					switch (dest_port->output_port)
					{
					case 1: enque_pkt(lists[0], pkt); break;
					case 2: enque_pkt(lists[1], pkt); break;
					case 3: enque_pkt(lists[2], pkt); break;
					case 4: enque_pkt(lists[3], pkt); break;
					}
				}
				else
				{
					free(pkt->data);
					free(pkt);
				}
			}
		}
		fclose(in_ports[i]);
	}

	// Sorting all lists
	for (i = 0; i < 4; i++)
	{
		sort_list(lists[i]->head_p0);
		sort_list(lists[i]->head_p1);
	}

	// Creating out port files using macros
	FILE *out_ports[4];
	char * out_file_name[4] = { OUTP1, OUTP2, OUTP3, OUTP4 };
	for (i = 0; i < 4; i++)
	{
		out_ports[i] = fopen(out_file_name[i], "w");
		assert(out_ports[i]);
	}

	// Filling outport files according to the lists
	for (i = 0; i < 4; i++)
	{
		packet* to_file = NULL;
		// First, priority 0
		while (lists[i]->head_p0)
		{
			to_file = deque_pkt(lists[i], 0);
			if (to_file)
				packet_write(out_ports[i], to_file);
			free(to_file->data);
			free(to_file);
		}
		// Second, priority 1
		while (lists[i]->head_p1)
		{
			to_file = deque_pkt(lists[i], 1);
			if (to_file)
				packet_write(out_ports[i], to_file);
			free(to_file->data);
			free(to_file);
		}
	}

    // Close output files
	for (i = 0; i < 4; i++)
		fclose(out_ports[i]);

	// Free bst and lists memory
	for (i = 0; i < 4; i++)
		free(lists[i]);
	free_tree(bst.root);
}
