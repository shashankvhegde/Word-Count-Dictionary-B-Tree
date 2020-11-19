
	/*Implementation of a word-count Dictionary*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define MAX 4
#define MIN 2
#define WORDSIZE 100

typedef char Key_type;
typedef int Data_type;
 
typedef enum{FALSE,TRUE}boolean;

typedef struct Treeentry{
	
	Key_type key[WORDSIZE];
	Data_type count;
	
}Treeentry;

typedef struct Treenode{
	
	int count; 
	Treeentry entry[MAX+1];
	struct Treenode* branch[MAX+2]; 
	
	
}Treenode;

int compare(Key_type k1[] ,Key_type k2[]){
	
	/*This function compares the keys*/
	
	if(strcmp(k1,k2) < 0){
		return -1 ;
	}
	else if(strcmp(k1,k2) == 0){
		return  0;
	}
	else{
		return 1;
	}
	
}

void traverse(Treenode *root){
	
	/*This function does inorder kind of traversal of the B-Tree and prints the keys and corresponding values */
	
	if(root != NULL){
		int l = root->count;
		int i;
		traverse(root->branch[0]);
		for(i = 0;i<l;i++){	
			printf("%s : %d\n",root->entry[i].key,root->entry[i].count);
			traverse(root->branch[i+1]);
			
		}
		
	}	
}

Treenode* makenode(){
	
	/*This function creates an empty node of the B-Tree*/
	
	Treenode *nptr;
	nptr = (Treenode*)malloc(sizeof(Treenode));
	if(nptr == NULL){
		printf("Error\n");
	}
	else{
		int i;
		for(i = 0;i<=MAX + 1;i++){
			strcpy(nptr->entry[i].key ,"");
			nptr->branch[i] = NULL;
		}
		nptr->branch[i] = NULL;
	}	
	return nptr;
}

boolean search(Treenode *root,Key_type key[],int *pos){
	
	/*This function employs Binary search to find key 'key' in the the root node 
	 *If the key is found in the same node (retval = TRUE)then *pos holds the value 
	 *of the index of the entry array in which the entry with the same key is present.
	 *Otherwise (retval = FALSE), *pos holds the index of the branch array from which the next search is to be continued.
	 */
	
	boolean retval; 
	if(root == NULL){
		printf("Empty Tree\n");
		retval = FALSE;
	}
	else{
		int l,low,high,mid;
		boolean found = FALSE;
		l = root->count;
		low = 0;
		high = l-1;
		while(low <= high && found == FALSE){
			mid = (low + high)/2;
			if(compare(key,root->entry[mid].key) < 0){
				high = mid - 1;
			}
			else if(compare(key,root->entry[mid].key) > 0){
				low = mid + 1;
			}
			else{
				found = TRUE;
				*pos = mid;
			}
		}
		if(found == FALSE){
			*pos = high + 1;
			retval = FALSE;
		}
		else{
			retval = TRUE;
		}
	}
	return retval;
}

void insert_in_node(Treenode *root,Treeentry entry,Treenode *right,int pos){
	
	/*Inserts entry 'entry' and its right subchild in the root node at index pos */
	
	int i;
	i = root->count;
	i -= 1;
	while(i>=0 && i >= pos){
		root->entry[i+1] = root->entry[i];
		root->branch[i+2] = root->branch[i+1];
		i--;
	}
	root->entry[i+1] = entry;
	root->branch[i+2] = right;
	
	root->count += 1;
	
}

boolean split_detector(Treenode *root){
	
	/*Detects whether the root node is full and needs a split*/
	
	if(root->count > MAX){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

Treeentry split(Treenode *root,Treenode **right){
	
	/*Splits root node into two nodes ,returns the median entry around which the split took place
	 * Also returns the pointer to the new node formed after split.(Treenode *right passed by reference)
	 */
	
	Treenode *r;
	Treeentry midentry;
	int median = (root->count - 1)/2;
	int i;
	midentry = root->entry[median];

	r = makenode();

	for(i = median+1;i<root->count;i++){
		r->entry[i-median-1] = root->entry[i];
		r->branch[i-median-1] = root->branch[i];
		
	} 
	r->branch[i-median-1] = root->branch[i];
	
	r->count = (root->count) - median - 1;
	root->count = median;
	*right = r;

	return midentry;
}

boolean Insertion(Treeentry *midentry,Treenode **right,Treenode *root,Treeentry entry){
	
	/*Recursively inserts entry in root node.
	 *midentry is the median entry which might come above from the next insertion call (on the child of root) into the root for insertion
	 *right pointer is the pointer to the new node formed if split took place in the next subtree(in the subsequent recursive call)
	 * Returns TRUE if a midentry is coming from the next subtree else returns FALSE.
	 */
	
	boolean retval = FALSE;
	int pos;
	if(root == NULL){
		*midentry = entry;
		*right = NULL;
		retval = TRUE;
	}
	else if(search(root,entry.key,&pos) == TRUE){
		root->entry[pos].count += 1;
		retval = FALSE;
	}
	else{
		if(Insertion(midentry,right,root->branch[pos],entry) == TRUE){
			insert_in_node(root,*midentry,*right,pos);
			if(split_detector(root) == TRUE){
				*midentry = split(root,right);
				retval = TRUE;
			}
		}
	}
	return retval;
}

Treenode* InsertionTree(Treenode *root,Treeentry entry){

	/*This function drives the recursive insertion process*/

	Treenode *nptr;
	Treeentry newentry;
	Treenode *right;
	boolean flag;
		
	flag = Insertion(&newentry,&right,root,entry);
	if(flag == TRUE){
		nptr = makenode();
		if(nptr != NULL){
			nptr->count = 1;
			nptr->entry[0] = newentry;
			nptr->branch[0] = root;
			nptr->branch[1] = right;
			root = nptr;	
		}//In case the older root split and produced a midentry in the Insertion function call above,it is made the new root
		
	}
	
	return root;
}

void merge(Treenode *prev,Treenode *root,int p){
	
	/*Used in case of Deletion .
	 *This is the case where the concerned node(from which deletion should take place,root) has both of its siblings with MIN entries
	 *In this case , the function merges root with one of its siblings 
	 */
	
	Treenode *right;
	right = prev->branch[p+1];
	int i;
	
	root->entry[root->count] = prev->entry[p];
	root->count += 1;
	i = 0;
	while(i < right->count){
		root->entry[root->count] = right->entry[i];
		root->branch[root->count] = right->branch[i];
		root->count += 1;
		i++;
	}
	root->branch[root->count] = right->branch[i];
	i = p;
	while(i< prev->count){
		
		prev->entry[i] = prev->entry[i+1];
		prev->branch[i+1] = prev->branch[i+2];
		i++;
	}
	prev->count -=1 ;
	free(right);
}

void fromleft(Treenode *prev,Treenode *root,int p){
	
	/*In this function, root borrows an entry from the parent and parent (prev) borrows the last entry of the left sibling of root*/
	
	
	Treenode *left = prev->branch[p-1];
	int i;
	for(i = (root->count) - 1;i>=0;i--){
		root->entry[i+1] = root->entry[i];
	
		root->branch[i+2] = root->branch[i+1];
		
	}
	
	root->branch[1] = root->branch[0];
	root->entry[0] = prev->entry[p-1];
	prev->entry[p-1] = left->entry[(left->count)-1];
	root->branch[0] = left->branch[left->count + 1];
	
	root->count += 1;
	left->count -= 1;
	//printf("fromleft done\n");
	
}

void fromright(Treenode *prev,Treenode *root,int p){
	
	/*In this function, root borrows an entry from the parent and parent (prev) borrows  the first entry of the right sibling of root*/
	
	Treenode *right;
	right = prev->branch[p+1];
	int i;
	
	root->entry[root->count] = prev->entry[p];
	root->count += 1;
	root->branch[root->count] = right->branch[0];
	
	prev->entry[p] = right->entry[0];
	right->branch[0] = right->branch[1];
	
	i = 1;
	while(i < right->count){
		
		right->entry[i-1] = right->entry[i];
		right->branch[i-1] = right->branch[i];
		i++;
	}
	right->count -= 1;	
}

void restore(Treenode **root,Treenode *prev,int p){
	
	/*This function checks when and which migrations to perform and performs the migrations accordingly*/
	
	if(prev != NULL){
		if(p == 0){
			if(prev->branch[1]->count > MIN){
				fromright(prev,*root,p);
			}
			else{
				merge(prev,*root,p);
			}
		}
		else if(p == prev->count){
			if(prev->branch[p-1]->count > MIN){
				fromleft(prev,*root,p);
			}
			else{
				merge(prev,prev->branch[p-1],p-1);
				*root = NULL;
			}
		}
		else{
			if(prev->branch[p-1]->count > MIN || prev->branch[p+1]->count > MIN){
				if(prev->branch[p-1]->count > MIN){
					
					fromleft(prev,*root,p);
				}
				else if(prev->branch[p+1]->count > MIN){
					fromright(prev,*root,p);
				}
			}
			else{
				merge(prev,*root,p);
			}
		}

	}
	else{
		if((*root)->count == 0){
			Treenode *temp;
			temp = *root;
			*root = (*root)->branch[0];
			free(temp);
		}
	}//This is the case when the children of root are merged and the root has no entry left in which case the root is changed.
	//printf("restore done\n");
}

void delete_from_node(Treenode *root,int pos){
	
	/*deletes entry at index pos from the root node*/
	
	int i;
	i = pos + 1;
	root->branch[pos] = root->branch[pos+1];
	while(i<(root->count)){
		root->entry[i-1] = root->entry[i];
		root->branch[i] = root->branch[i+1];
		i++;
	}
	root->count -= 1;
}


boolean isleaf(Treenode *root){
	
	/*Return TRUE if the root is a leaf node else returns FALSE*/

	if(root->branch[0] == NULL){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

void successor(Treenode *root,int pos){
	
	/*Finds the next successor of the entry at index pos in the root node and replaces the predecessor with its successor*/
	
	Treenode *nptr;
	nptr = root->branch[pos+1];
	while(isleaf(nptr) == FALSE){
		nptr = nptr->branch[0];
	}	
	root->entry[pos] = nptr->entry[0];
}



Treenode* DeletionTree(Treenode *root,Treenode *prev,Key_type key[],int p){

	/*Deletes an entry with key 'key' from the B-tree rooted at root.*/

	int pos;
	if(root == NULL){
		printf("Empty Tree\n");
	}
	else{
		if(search(root,key,&pos) == TRUE){
			if(isleaf(root) != TRUE){
				successor(root,pos);
				root->branch[pos+1] = DeletionTree(root->branch[pos+1],root,root->entry[pos].key,pos+1);
			}
			else{
				delete_from_node(root,pos);
			}
		}
		else{
			root->branch[pos] = DeletionTree(root->branch[pos],root,key,pos);
		}
		if(root->count < MIN){
			restore(&root,prev,p);
		}
	}	
	return root;
	
}
/***********************************************************************************************/
void printfirstandlast(Treenode *root){
	
	/*Prints the first and last words as per lexicographical order in the Dictionary implemeted by B-Tree rooted at root*/
	
	if(root != NULL){
		Treenode *nptr = root;
		while(nptr->branch[0] != NULL){
			nptr = nptr->branch[0];
		}
		printf("First Word: %s\n",nptr->entry[0].key);
		nptr = root;
		while(nptr->branch[nptr->count] != NULL){
			nptr = nptr->branch[nptr->count];
		}
		printf("Last Word: %s\n",nptr->entry[(nptr->count) - 1].key);
	}
}

int getmin(Treenode *root){
	
	/*Returns the minimum count of any word in the Dictionary implemeted by B-Tree rooted at root*/
	
	int min = INT_MAX;
	int m;
	if(root != NULL){

		int i;
		for(i = 0;i<=root->count;i++){
			m = getmin(root->branch[i]);
			if(m < min){
				min = m;
			}
		}
		for(i = 0;i<root->count;i++){
			if(root->entry[i].count < min){
				min = root->entry[i].count;
			}
		}
	}
	return min;
}

void leastoccurring(Treenode *root,int m){
	
	/*Prints the least occuring words in the Dictionary implemeted by B-Tree rooted at root*/

	if(root != NULL){
		int i;
		for(i = 0;i<root->count;i++){
			leastoccurring(root->branch[i],m);
	
			if(root->entry[i].count == m){
				printf("%s : %d\n",root->entry[i].key,root->entry[i].count);
			}
			
		}
		leastoccurring(root->branch[i],m);
	}

}

void greater_than_k(Treenode *root,int k){
	
	/*Prints the words with count > k in the Dictionary implemeted by B-Tree rooted at root*/
	
	if(root != NULL){
		int i;
		for(i = 0;i<root->count;i++){
			greater_than_k(root->branch[i],k);
			if(root->entry[i].count > k){
				printf("%s : %d\n",root->entry[i].key,root->entry[i].count);
			}			
		}
		greater_than_k(root->branch[i],k);
	}
}

void rangesearch(Treenode *root,Key_type lb[],Key_type ub[]){
	
	/*Prints the words within the range lb and ub in the Dictionary implemeted by B-Tree rooted at root*/
	
	if(root != NULL){
		int l = root->count;
		int i;
		//rangesearch(root->branch[0],lb,ub);
		for(i = 0;i<l;i++){
			
			rangesearch(root->branch[i],lb,ub);
			
			if(compare(root->entry[i].key,lb) >=0 && compare(root->entry[i].key,ub) <= 0){
				printf("%s : %d\n",root->entry[i].key,root->entry[i].count);
			}
	
		}
		rangesearch(root->branch[i],lb,ub);
	}
}

void traverse_store(Treenode *root,FILE *fp){
	
	/*Traverses the B-Tree rooted at root in inorder like fashion and stores the words and counts in the file Dictionary.txt*/

	char s[4*WORDSIZE] = "";
	if(root != NULL){
		int l = root->count;
		int i;
		traverse_store(root->branch[0],fp);
		for(i = 0;i<l;i++){	
			fprintf(fp,"\t%s\t%d\n",root->entry[i].key,root->entry[i].count);
			traverse_store(root->branch[i+1],fp);
			
		}
		
	}
		
}
/***********************************************************************************************/
int main() {
		
	int flag = 1,choice = 0,min,k;
	Treeentry n;
	n.count = 0;
	Treenode *root = NULL;
	Key_type lb[WORDSIZE],ub[WORDSIZE];
	Key_type s[WORDSIZE*4],word[WORDSIZE];
	int count;
	
	FILE *fp;

	fp = fopen("Dictionary.txt","r");
	
	if(fp != NULL){
		 
		while(fgets(s,4*WORDSIZE,fp)!=NULL){
			sscanf(s,"\t%s\t%d\n",word,&count);
			strcpy(n.key,word);
			n.count = count;
			root = InsertionTree(root,n);
		}
		
	}	
	fclose(fp);
	
	while(choice != 8){
		
		printf("\nThe choices are:\n");
		printf("1.Insertion\n");
		printf("2.Deletion\n");
		printf("3.Print in lexicographical order\n");
		printf("4.Print First and Last word\n");
		printf("5.Print Least occurring words\n");
		printf("6.Print words occurring more than k times\n");
		printf("7.Print words in a range\n");
		printf("8.Exit\n");
		
		printf("\nEnter choice:\n");
		
		scanf("%d",&choice);
		switch(choice){
			case 1:
				printf("Enter a word\n");
				scanf("%s",(n.key));
				n.count = 1;
				root = InsertionTree(root,n);
	
				break;
			case 2:
				printf("\nEnter the word to be deleted\n");
				scanf("%s",(n.key));
				if(strcmp(n.key ,"0") == 0){
					flag = 0;
				} 
				else{
					root = DeletionTree(root,NULL,n.key,0);
				}
				break;			
			case 3:
				traverse(root);
				break;			
			case 4:
				printfirstandlast(root);
				break;
			case 5:
				min = getmin(root);
				leastoccurring(root,min);
				break;
			case 6:
				printf("Enter k\n");
				scanf("%d",&k);
				greater_than_k(root,k);
				break;
			case 7:
				printf("\nEnter Lower bound\n");
				scanf("%s",lb);
				printf("Enter Upper bound\n");
				scanf("%s",ub);
				rangesearch(root,lb,ub);
				break;
			case 8:
				printf("Exit!");
					fp = fopen("Dictionary.txt","w");
					traverse_store(root,fp);
					fclose(fp);
				break;
			default:
				printf("Invalid choice\n");
				break;				
			
				
		}
	}	 
	return 0;
	
}

