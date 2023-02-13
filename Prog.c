#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"
#include <stdlib.h>
#include <assert.h>

void packet_read(FILE *fp, packet *pkt) // file is already open
{
	if (fgetc(fp) == '\n' && fgetc(fp) == EOF)
	{
		pkt = NULL;
		return;
	}
	fseek(fp, -1, SEEK_CUR);

	int i;
	fscanf(fp, "%d %d %d %d %d", &pkt->time, (int*)&pkt->Da, (int*)&pkt->Sa, (int*)&pkt->Prio, (int*)&pkt->Data_length);
	assert(pkt->data = (unsigned char*)malloc(pkt->Data_length * sizeof(unsigned char)));

	for (i = 0; i < pkt->Data_length; i++)
		fscanf(fp, "%hhu", (&pkt->data[i]));

	fscanf(fp, "%d", (int*)&pkt->Checksum);
}

void packet_write(FILE *fp, const packet *pkt)
{
	int i;
	fprintf(fp, "%d ", pkt->time);
	fprintf(fp, "%d ", pkt->Da);
	fprintf(fp, "%d ", pkt->Sa);
	fprintf(fp, "%d ", pkt->Prio);
	fprintf(fp, "%d ", pkt->Data_length);
	for (i = 0; i < pkt->Data_length; i++)
		fprintf(fp, "%d ", pkt->data[i]);
	fprintf(fp, "%d\n", pkt->Checksum);
}

bool checksum_check(const packet *pkt)
{
	int i;
	unsigned char res;
	res = (pkt->Da) ^ (pkt->Sa) ^ (pkt->Prio) ^ (pkt->Data_length);
	for (i = 0; i < pkt->Data_length; i++)
		res = res ^ pkt->data[i];
	if (res == pkt->Checksum)
		return TRUE;
	else
		return FALSE;
}

S_node* create_node(unsigned char da, char Pout)
{
	S_node *temp;
	temp = (S_node *)calloc(1, sizeof(S_node));
	temp->da = da;
	temp->output_port = Pout;
	temp->left = temp->right = NULL;
	return temp;
}

void addItemToTreeRec(S_node *root, unsigned char da, char output_port)
{
	if (da <= root->da) // left subtree
	{
		if (root->left == NULL)
			root->left = create_node(da, output_port);
		else
			addItemToTreeRec(root->left, da, output_port);
	}

	else //right subtree
	{
		if (root->right == NULL)
			root->right = create_node(da, output_port);
		else
			addItemToTreeRec(root->right, da, output_port);
	}
}

S_node *add_route(S_node *root, char da, char output_port)
{
	if (root == NULL)
		root = create_node(da, output_port);
	else
		addItemToTreeRec(root, da, output_port);
	return root;
}

S_node* LowestDa(S_node* node) // we use this function to arrange the tree properly
{
	S_node* current = node;
	// loop down to find the leftmost leaf 
	while (current && current->left != NULL)
		current = current->left;
	return current;
}

S_node *delete_route(S_node *root, char da)
{
	S_node *temp;
	if (root == NULL) return root;

	//if da < da in root, it's on the left subtree
	if (da < root->da)
		root->left = delete_route(root->left, da);

	//if da > da in root, it's on the right subtree
	else if (da > root->da)
		root->right = delete_route(root->right, da);
	//if da = da in root, delet root and find nodee to replace root
	else
	{
		// node with only one child or no child 
		if (root->left == NULL)
		{
			temp = root->right;
			free(root);
			root->da = NULL;
			return temp;
		}
		else if (root->right == NULL)
		{
			temp = root->left;
			free(root);
			root->da = NULL;
			return temp;
		}
		temp = LowestDa(root->right); // using this function to find the lowest Da to be arranged by
		root->da = temp->da;
		root->output_port = temp->output_port;
		root->right = delete_route(root->right, temp->da);
	}
	return root;
}

S_node *search_route(const S_node *root, char da)
{
	unsigned char temp = da;
	S_node* item;
	if (!root || (root->da == temp))
		return root;

	if (item = search_route(root->left, temp))
		return item;
	return search_route(root->right, temp);
}

void print_routing_table(const S_node  *root)
{
	if (!root) return;
	print_routing_table(root->left);
	printf("%d ", root->da);
	print_routing_table(root->right);
}

S_node *build_route_table(FILE *fp, S_node *root)
{
	unsigned char check, *da, *out_port;
	//build tree
	fseek(fp, 0, SEEK_SET);
	while (!feof(fp))
	{
		check = fgetc(fp);
		if (check == 'a')
		{
			fscanf(fp, "%d %d", &da, &out_port);
			root = add_route(root, da, out_port);
		}
		else if (check == 'd')
		{
			fscanf(fp, "%d", &da);
			out_port = NULL;
			root = delete_route(root, da);
		}
		else
			break;
		fseek(fp, 2, SEEK_CUR);
	}
	printf("The values of the bst are:\n");
	print_routing_table(root);
	printf("\n");
	return root;
}

S_Out_Qs_mgr* add_last(S_pkt* packet_node, S_Out_Qs_mgr* List)
{

	if (packet_node->pkt->Prio == 0)
	{
		if (List->head_p0 == NULL)
			List->head_p0 = List->tail_p0 = packet_node;
		else
		{
			List->tail_p0->next = packet_node;
			List->tail_p0 = List->tail_p0->next;
		}
	}
	else
	{
		if (List->head_p1 == NULL)
			List->head_p1 = List->tail_p1 = packet_node;
		else
		{
			List->tail_p1->next = packet_node;
			List->tail_p1 = List->tail_p1->next;
		}
	}
	return List;
}

void enque_pkt(S_Out_Qs_mgr *QM_ptr, packet *pkt)
{

	if (pkt == NULL)
		return;
	if (checksum_check(pkt))
	{
		S_pkt *packet_node;
		packet_node = (S_pkt*)calloc(1, sizeof(S_pkt));
		assert(packet_node);
		//init packet node:
		packet_node->pkt = pkt;
		packet_node->next = NULL;
		QM_ptr = add_last(packet_node, QM_ptr);
	}
	else
	{
		free(pkt->data);
		free(pkt);
	}
}

packet *deque_pkt(S_Out_Qs_mgr *QM_ptr, char priority)
{
	packet *first;
	if (priority == 0)
	{
		if (QM_ptr->head_p0 == NULL)
		{
			printf("\nList is Empty ...");
			return NULL;
		}
		else
		{
			first = QM_ptr->head_p0->pkt;
			QM_ptr->head_p0 = QM_ptr->head_p0->next;
		}
		return first;
	}
	else
	{
		if (QM_ptr->head_p1 == NULL)
		{
			printf("\nList is Empty ...");
			return NULL;
		}
		else
		{
			first = QM_ptr->head_p1->pkt;
			QM_ptr->head_p1 = QM_ptr->head_p1->next;

		}
		return first;
	}
}

void sort_list(S_pkt* head)
{
	S_pkt* location, *next;
	int changed; //location change flag

	if (head)
	{
		do
		{
			changed = 0;
			location = head;
			next = location->next;
			while (next)
			{
				if (location->pkt->time > next->pkt->time) //if next packet comes before current packet 
				{
					packet *temp = location->pkt;
					location->pkt = next->pkt;
					next->pkt = temp;
					changed = 1;		//raise flag
				}
				location = next;
				next = next->next;
			}
		} while (changed);
	}
}

void free_tree(S_node * node)
{
	if (node != NULL)
	{
		free_tree(node->right);
		free_tree(node->left);
		free(node);
	}
}