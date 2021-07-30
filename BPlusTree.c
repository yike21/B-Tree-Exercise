#include "BPlusTree.h"

static KEYTYPE unavailable = INT_MIN;
static void copyData(DATATYPE *dest, DATATYPE *source);

static PBTreeNode createNode(int isLeaf)
{
  PBTreeNode new_node;
  new_node = (PBTreeNode)calloc(1,sizeof(struct BPlusTreeNode));

  new_node->leaf = isLeaf;
  new_node->num = 0;
  new_node->next = NULL;
  for(int i = 0; i<DEGREE_2 ; i++)
  {
    new_node->data[i] = NULL;
  }

  for (int i=0; i < DEGREE_2 ; i++)
  {
    new_node->key[i] = unavailable;
    new_node->children[i] = NULL;
  }

  return new_node; 
} 

extern BPTree initialize()
{
  BPTree btree;

  btree = (BPTree)calloc(1,sizeof(struct BPlusTree));
  btree->root = createNode(true);
  btree->first = btree->root;
  btree->total_key_num = 0;

  //DATATYPE data = { "head" };
  //insert(btree, unavailable, data);
  //insert(btree, 15, data);

  return btree;
}


static PBTreeNode findMostLeft(PBTreeNode x)
{
  PBTreeNode temp = x;
  while(temp != NULL && !temp->leaf)
  {
    temp = temp->children[0];
  }

  return temp;
}

static PBTreeNode findMostRight(PBTreeNode x)
{
  PBTreeNode temp = x;
  while(temp != NULL && !temp->leaf)
  {
    temp = temp->children[temp->num-1];
  }

  return temp;
}

static int findUpperPosition(PBTreeNode x, KEYTYPE targetKey)
{
  int low = 0;
  int high = x->num-1;
  int mid;

  while(low < high)
  {
    mid = (low + high + 1) / 2;
   if(targetKey < x->key[mid])
    {
      high = mid-1;
    }
    else
    {
      low = mid;
    }
  }

  return high;
}

extern DATATYPE* search(BPTree T, KEYTYPE key)
{
  PBTreeNode pnode = T->root;
  KEYTYPE findKey = unavailable;
  int position = findUpperPosition(pnode, key);

  while(!pnode->leaf)
  {
    pnode = pnode->children[position];
    position = findUpperPosition(pnode, key);
  }

  if(pnode->key[position] == key){
    return pnode->data[position];
  }

  return NULL;
}

extern BPTree update(BPTree T, KEYTYPE key, DATATYPE newData)
{
  DATATYPE *data = search(T, key);
  if(data == NULL)
  {
    printf("update error!");
  }
  copyData(data, &newData);

  return T;
}


static void btreeSplitChild(PBTreeNode x, int i) 
{ 
	PBTreeNode y = x->children[i];
	PBTreeNode z = createNode(y->leaf);
 
	z->num = DEGREE;
 
	int j = 0;
	for (j = 0; j < DEGREE ; j++) 
  {
		z->key[j] = y->key[j+DEGREE];
	}
	if (!y->leaf) 
  {
		for (j = 0; j < DEGREE ; j++) 
    {
			z->children[j] = y->children[j+DEGREE];
		}
	}
  else
  {
    z->next = y->next;
    y->next = z;
    for(j = 0; j<DEGREE ; j++)
    {
      z->data[j] = y->data[j+DEGREE];
    }
  }
	y->num = DEGREE;

	for (j = x->num; j > i+1 ; j--) 
  {
    x->key[j] = x->key[j-1];
		x->children[j] = x->children[j-1];
	}
  x->key[i+1] = z->key[0];
	x->children[i+1] = z;
	x->num += 1;
}

static void copyData(DATATYPE *dest, DATATYPE *source)
{
  strcpy(dest->idcard, source->idcard);
}

static KEYTYPE btreeInsertNonfull(PBTreeNode x, KEYTYPE k, DATATYPE data) 
{
 
	int i = x->num -1;
 
	if (x->leaf) 
  {
    x->data[i+1] = (DATATYPE*)calloc(1,sizeof(DATATYPE));
		while (i >= 0 && x->key[i] > k) 
    {
			x->key[i+1] = x->key[i];
      //x->data[i+1] = (DATATYPE*)calloc(1,sizeof(DATATYPE));
      copyData(x->data[i+1], x->data[i]);
			i--;
		}
    
    //x->data[i+1] = (DATATYPE*)calloc(1,sizeof(DATATYPE));
    copyData(x->data[i+1], &data);    
		x->key[i+1] = k;
		x->num += 1;

    return x->key[0];
	} 
  else 
  {
		while (i >= 0 && x->key[i] > k) i--;    //这行代码，无法处理在key[i]中比插入比key[i]的值
    if(i == -1) i+=1;    //用这行代码作为补充

		if (x->children[i]->num == DEGREE_2) {
			btreeSplitChild(x, i);
			if (k > x->key[i+1]) i++;
		}

		x->key[i] = btreeInsertNonfull(x->children[i], k, data);
    return x->key[0];
	}
}

extern BPTree insert(BPTree T, KEYTYPE key, DATATYPE data) 
{
  T->total_key_num += 1;
	PBTreeNode r = T->root;
	if (r->num == DEGREE_2) {
		
		PBTreeNode pnode = createNode(false);
		T->root = pnode;
 
		pnode->children[0] = r;
    pnode->key[0] = r->key[0];
    pnode->num = 1;
 
		btreeSplitChild(pnode, 0);
 
		int i = 0;
		if (pnode->key[i+1] < key) i++;
		btreeInsertNonfull(pnode->children[i], key, data);
		
	} else {
		btreeInsertNonfull(r, key, data);
	}

  return T;
}


static void showBPNode(PBTreeNode pnode, int layer)
{
  if(pnode == NULL)
    printf("Node is empty!\n");
  else
  {
    PBTreeNode node = pnode;

    printf("\nlayer is %d, key_num is %d, is_leaf is %d\n", layer++, node->num, node->leaf);
    printf("keys are: ");
    for(int k = 0; k<node->num ; k++)
    {
      printf("%d ",node->key[k]);
    }
    printf("\n");

    if(node->leaf)
    {
      printf("dataes are: \n");
      for(int k = 0; k<node->num ; k++)
      {
        printf("%d: %s\n", node->key[k], node->data[k]->idcard);
      }
    }
    else
    {
      for(int k = 0; k<node->num ; k++)
      {
        showBPNode(node->children[k], layer);
      }
    }
  }
}

extern void showBPlusTree(BPTree T)
{
  if(T->root == NULL)
  {
    printf("Tree is empty!");
  }
  else
  {
    showBPNode(T->root, 0);
  }
}


extern KEYTYPE* travelN(BPTree T, int num)
{
  PBTreeNode pnode = T->first;
  KEYTYPE *keys = (KEYTYPE*)calloc(num,sizeof(KEYTYPE));    //初始化的 -1,head 不计入total_key_num中 
  int offset = 0;

  while(pnode != NULL)
  {
    for(int i = 0; i+offset<num+1 && i<pnode->num ; i++)
    {
      keys[i+offset] = pnode->key[i];
    }
    offset += pnode->num;
    pnode = pnode->next;
  }

  return keys;
}

extern KEYTYPE* travel(BPTree T)
{
  return travelN(T,T->total_key_num);
}

extern DATATYPE* travelDataN(BPTree T, int num)
{
  PBTreeNode pnode = T->first;
  DATATYPE* dataes = (DATATYPE*)calloc(num, sizeof(DATATYPE));
  int offset = 0;

  while(pnode != NULL)
  {
    for(int i = 0; i<pnode->num ; i++)
    {
      copyData(dataes+(i+offset), pnode->data[i]); 
    }

    offset += pnode->num;
    pnode = pnode->next;
  }

  return dataes;
}

extern DATATYPE* travelData(BPTree T)
{
  return travelDataN(T, T->total_key_num);
}


static void deleteButNotDestroyBPlusNode(PBTreeNode x)
{
  free(x);
  x = NULL;
}

static void btreeMergeChild(PBTreeNode x, int index)
{
  PBTreeNode y = x->children[index];
  PBTreeNode z = x->children[index+1];
  if(y->num > DEGREE || z->num > DEGREE)
  {
    printf("Merge error!");
    return;
  }

  y->num = DEGREE_2;
  if(y->leaf)
  {
    for(int j = 0; j<DEGREE ; j++)
    {
      y->key[j+DEGREE] = z->key[j];
      //copyData(y->data[j+DEGREE], z->data[j]);    //此处有错误2
      y->data[j+DEGREE] = z->data[j];    //修正错误2
    }
    y->next = z->next;
  }
  else
  {
    for(int i = 0; i<DEGREE ; i++)
    {
      y->key[i+DEGREE] = z->key[i];
      y->children[i+DEGREE] = z->children[i];
    }
  }

  deleteButNotDestroyBPlusNode(z);

  for(int j = index+1; j<x->num-1 ; j++)
  {
    x->key[j] = x->key[j+1]; 
    x->children[j] = x->children[j+1];
  }
  x->num -= 1;
}

#if 0
static void replaceFirstKeyAndDataWithLeftSibling(PBTreeNode x, KEYTYPE key, DATATYPE data)
{
  x->key[0] = key;
  if(x->leaf)
  {
    copyData(x->data[0], &data);
    return ;
  }
  else
  {
    replaceFirstKeyAndDataWithLeftSibling(x->children[0], key, data);
  }
}

static void replaceKeyAndDataWithRightSibling(PBTreeNode x, KEYTYPE key, DATATYPE data)
{
  
}
#endif

static void moveKeyFromAToB(PBTreeNode x, int a, int b);

static KEYTYPE btreeRemoveGTDegree(PBTreeNode x, KEYTYPE key)
{
  int position = findUpperPosition(x, key);

  if(x->leaf)
  {
    if(x->key[position] != key)
    {
      printf("remove leaf error!\n");
    }

    //free(x->data[position]);    //这里存在错误1  .1
    free(x->data[position]);    //搭配下面的更改2
    for(int i = position; i<x->num-1 ; i++)
    {
      x->key[i] = x->key[i+1];
      //copyData(x->data[i], x->data[i+1]);    //这里是导致错误1的原因  .3
      x->data[i] = x->data[i+1];    //此处为更改2第一步
    }
    x->data[x->num-1] = NULL;    //此处为更改2第二步
    //free(x->data[x->num-1]);    //修正错误1  .2

    x->num -= 1;
    return x->key[0];
  }
  else
  {
    if(x->children[position]->num > DEGREE)
    {
      x->key[position] = btreeRemoveGTDegree(x->children[position], key); 
    }
    else if(x->children[position]->num == DEGREE)
    {
      if(position - 1 >= 0 && x->children[position-1]->num > DEGREE)
      {
        moveKeyFromAToB(x, position-1, position);
      }
      else if(position+1 < x->num && x->children[position+1]->num >DEGREE)
      {
        moveKeyFromAToB(x, position+1, position);
      }
      else if(position-1 >= 0 && x->children[position-1]->num == DEGREE)
      {
        btreeMergeChild(x, position-1); position -= 1;
      }
      else if(position+1 <x->num && x->children[position+1]->num == DEGREE)
      {
        btreeMergeChild(x, position);
      }
      
      x->key[position] = btreeRemoveGTDegree(x->children[position], key);
    }

    return x->key[0];
  }
}

static void moveKeyFromAToB(PBTreeNode x, int a, int b)
{
  PBTreeNode p;
  KEYTYPE tmpKey;
  DATATYPE tmpData;
  if(a < b)
  {
    p = findMostRight(x->children[a]);
    tmpKey = p->key[p->num-1];
    copyData(&tmpData, p->data[p->num-1]);
    btreeRemoveGTDegree(x->children[a], tmpKey);
    btreeInsertNonfull(x->children[b], tmpKey, tmpData);
  }
  else if(a > b)
  {
    p = findMostLeft(x->children[a]);
    tmpKey = p->key[0];
    copyData(&tmpData, p->data[0]);
    btreeRemoveGTDegree(x->children[a], tmpKey);
    btreeInsertNonfull(x->children[b], tmpKey, tmpData);    
  }
}

extern BPTree removeKey(BPTree T, KEYTYPE key)
{
  DATATYPE * data = search(T, key);
  if(data == NULL)
  {
    printf("Remove error! These isn't the key!");
    return T;
  }

  T->total_key_num -= 1; 
  PBTreeNode r = T->root;
  btreeRemoveGTDegree(r, key);

  return T;
}
