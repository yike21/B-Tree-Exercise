# B-Tree
### **引入**  
1. 索引是一种数据结构，用于帮助我们在大量数据中快速定位到我们想要查找的数据。
2. 索引在mysql数据库中分三类： B+树索引、Hash索引、全文索引。
![Alt](pic/12.png)  
* 从图中可以看到，我们为user表（用户信息表）建立了一个二叉查找树的索引。图中的圆为二叉查找树的节点，节点中存储了键(key)和数据(data)。  
* 键对应user表中的id，数据对应user表中的行数据。二叉查找树的特点就是任何节点的左子节点的键值都小于当前节点的键值，右子节点的键值都大于当前节点的键值。 顶端的节点我们称为根节点，没有子节点的节点我们称之为叶节点。   
* **如果我们需要查找id=12的用户信息，利用我们创建的二叉查找树索引，查找流程如下：** 
  1. 将根节点作为当前节点，把12与当前节点的键值10比较，12大于10，接下来我们把当前节点>的右子节点作为当前节点。 

  2. 继续把12和当前节点的键值13比较，发现12小于13，把当前节点的左子节点作为当前节点。 

  3. 把12和当前节点的键值12对比，12等于12，满足条件，我们从当前节点中取出data，即id=12,name=xm。

利用二叉查找树我们只需要3次即可找到匹配的数据。如果在表中一条条的查找的话，我们需要6次才能找到。 

因为内存的易失性。一般情况下，我们都会选择将user表中的数据和索引存储在磁盘这种外围设备中。  

但是和内存相比，从磁盘中读取数据的速度会慢上百倍千倍甚至万倍，所以，我们应当尽量减少从磁盘中读取数据的次数。 另外，从磁盘中读取数据时，都是按照磁盘块来读取的，并不是一条一条的读。 

**如果我们能把尽量多的数据放进磁盘块中，那一次磁盘读取操作就会读取更多数据，那我们查找数据的时间也会大幅度降低。**   

********************************
**用两张图来直观看B-Tree和B+Tree**  
B树：  
![Alt](pic/10.png)  
模拟查找关键字29的过程：

1. 根据根节点找到磁盘块1，读入内存。【磁盘I/O操作第1次】

2. 比较关键字29在区间（17,35），找到磁盘块1的指针P2。
根据P2指针找到磁盘块3，读入内存。【磁盘I/O操作第2次】

3. 比较关键字29在区间（26,30），找到磁盘块3的指针P2。
根据P2指针找到磁盘块8，读入内存。【磁盘I/O操作第3次】
在磁盘块8中的关键字列表中找到关键字29。

********************************
B+树：
![Alt](pic/11.png)   
* 事实上可能只是为每个关键字存放一个指针，这个指针指向存放该关键字的卫星数据的磁盘页。  
* B+树把所有的卫星数据信息存储在叶结点中，内部结点只存放关键字和孩子指针，因此最大化了内部结点的分支因子。  
* 接下来的伪代码隐含地假设了，当一个关键字从一个结点移动到另一个结点时，与关键字相联系的卫星数据/指向卫星数据的指针，都会随着关键字一起移动。  


**********************************
### **B树的定义：一棵B树T是具有以下性质的有根树（根为T.root）**
1. 每个节点x有下面属性:  
   * ***x.n*** 是当前存储在结点x中的关键字个数。  
   * ***x.n***个关键字本身 ***x.key[1], x.key[2], ..., x.key[x.n]*** 以非降序存放。  
   * ***x.leaf*** 是一个布尔值，如果x是叶节点，则为TRUE；如果x为内部节点，则为false。

2. 每个内部节点x还包括x.n+1个指向其孩子的指针 ***x.c[1], x.c[2], ..., x.c[n+1]*** 。叶节点没有孩子，所以它们的c[i]属性没有定义。

3. 关键字 ***x.key[i]*** 对存储在各子树中的关键字范围加以分隔：如果 ***k[i]*** 为任意一个存储在以 ***x.c[i]*** 为根的子树中的关键字，那么  
   ***k[1] <= x.key[1] <= k[2] <= ... <= x.key[x.n] <= k[x.n+1]***

4. 每个叶节点具有相同的深度，即树的高度h。

5. 每个节点所包含的关键字个数有上界和下界。有一个被称为B树的 **最小度数** 的固定整数 ***t***（t>=2) 来表示这些界。
   * 除了根结点以外的每个结点必须至少有t-1个关键字。因此，除了根结点以外的每个内部结点至少有t个孩子。如果树为非空，根结点至少有一个关键字。
   * 每个结点至多可包含2t-1个关键字。因此，每个内部结点至多可有2t个孩子。当一个结点恰有2t-1个关键字时，称该结点为满（full)的。

*******
### B树的高度  
**对任意一棵包含 n 个关键字、高度为 h、最小度数为 t 的B树T，有**    
***h ≤ logt (n+1)/2***  
![Alt](pic/1.png)  

*******
### B树的基本操作

#### 搜索B树
![Alt](pic/2.png)  

在 B-TREE-SEARCH 过程访问的磁盘页面数为O（h）= O(logt n),其中h为树高，n为B树中所含关键字个数。  

由于x.n < 2t, 所以第2-3行的while循环在每个结点中花费时间为O（t),总的CPU时间为 O(t x h) = O(t x logt n)。

********************************
#### 向B树中插入一个关键字
在插入结点时，每个内部结点的关键字x.n 范围是t-1 ~ 2t-1 ,对于一个满了的叶结点，需要按其中间关键字y.key[i]分裂为两个各含t-1 个关键字的结点，中间关键字被提升到y的父结点，以标识两颗新树的划分点。

********************************
#### 分裂树中的结点
在插入结点过程中，并不是等到找出实际要分裂的满结点才做分裂。  

当沿着树往下查找新的关键字所属位置时，就分裂沿途遇到的每个满结点（包括叶结点）。这样每当要分裂一个满结点时，就能确保它的父结点不是满的。  

过程B-TREE-SPLIT-CHILD的输入是一个非满的内部结点x，和一个使x.c[i]为 x 的满子结点的下标i。  
![Alt](pic/3.png)
![Alt](pic/4.png)  
第1-9行创建结点z，并将y的t-1个关键字以及相应的t个孩子给它。第10行调整y的关键字个数。最后，第11-17行将z插入为x的一个孩子，并提升y的中间关键字到x来分隔y和z，然后调整x的关键字个数。 

********************************
#### 以沿树单程下行方式向B树插入关键字
在一棵高度为 h 的B树 T 中，以沿树单程下行方式插入一个关键字 k 的操作需要O（h)次磁盘存取，所需要的CPU时间为O（th）=O(t x logt n)。  

过程B-TREE-INSERT利用B-TREE-SPLIT-CHILD来保证递归始终不会降至一个满结点上。  

![Alt](pic/5.png)  

![Alt](pic/6.png)  

第3-9行处理了根节点满了的情况：根结点分裂为两个节点，新建的结点s成为根。根节点分裂是增加B树高度的唯一途径。  

辅助的递归过程B-TREE-INSERT-NONFULL将关键字 k 插入结点 x ，要求假定在调用过程时 x 是非满的。操作B-TREE-INSERT和递归操作B-TREE-INSERT-NONFULL保证了这个假设成立。  

![Alt](pic/7.png)  

第3-8行处理x是叶结点的情况，将关键字k插入x。如果x不是叶结点，则必须将k插入以内部结点x为根的子树中适当的叶结点中去。  

第9-11行决定向x的哪个子结点递归下降。第13-16行保证了该程序始终不会降至一个满结点上。第17行递归地将k插入合适的子树中。 

![Alt](pic/8.png)   

********************************
### 从B树中删除关键字
当删除一个关键字时，必须防止因删除操作而导致树的结构违反B树的性质。  

设计一个过程B-TREE-DELETE从以x为根的子树中删除关键字k。这个火证必须保证无论何时，节点x递归调用自身时，x中关键字个数至少为最小度数t。  

这个条件要求比通常B树中的最少关键字个数多一个以上，这就需要有时在递归下降至子结点前，需要把一个关键字移到子结点中。  

这个加强条件允许在一趟下降过程中，就可以将一个关键字从B树中删除。  

#### 从B树中删除关键字的各种情况：
1. 如果关键字 k 在结点 x 中，并且 x 是叶结点，则从 x 中删除 k 。  

2. 如果关键字 k 在结点 x 中，并且 x 是内部结点，则做以下操作：  
   **2a.** 如果结点x中前于k的子结点y至少包含t个关键字，则找出k在以y为根的子树中的前驱k'。递归地删除k',并在x中用k'代替k。  
   **2b.** 对称地，如果y有少于t个关键字，则检查结点x中后于k的子结点z。如果z至少有t个关键字，则找出k在以z为根的子树中的后继k'。递归地删除k'，并在x中用k'代替k。  
   **2c.** 否则，如果y和z都只含有t-1个关键字，则将k和z的全部合并进y，这样x就失去了k和指向z的指针，并且现在y包含2t-1个关键字。然后释放z并递归地从y中删除k。  

3. 如果关键字k当前不在内部结点x中，则确定必包含k的子树的根x.c[i]。如果x.c[i]只有 t-1个关键字，则必须执行步骤3a或3b来保证降至一个至少包含t个关键字的结点。然后通过对x的某个合适子结点进行递归而结束。  
   **3a.** 如果x.c[i]只含有t-1个关键字，但是它的一个相邻兄弟至少包含t个关键字，则将x中的某一个关键字降至x.c[i]，将x.c[i]的相邻左兄弟或右兄弟的一个关键字升至x，将该兄弟中相应的孩子指针移到x.c[i]中。这样使得x.c[i]增加了一个额外的关键字。  
   **3b.** 如果x.c[i]以及x.c[i]的所有相邻兄弟都只包含t-1个关键字，则将x.c[i]与一个兄弟合并，将x的一个关键字移到新合并的结点，使之成为该结点的中间关键字。    
 
![Alt](pic/9.png)  
由于一棵B树中的大部分关键字都在叶节点中，我们可以预期在实际中，删除操作最经常用于从叶结点中删除关键字。这样B-TREE-DELETE过程只要沿树下降一趟即可。对一棵高度为h的B树，只需要O(h)次磁盘操作。 

# B+Tree
### 回看B树的另一种定义：一棵m阶B树T需要遵循以下规则：
1. 每个结点最多有m个子结点。
2. 每个非叶子结点（除了根）具有至少⌈ m/2⌉子结点。
3. 如果根不是叶节点，则根至少有两个子节点。
4. 具有k个子节点的非叶节点包含k-1个键。
5. 所有叶子都出现在同一水平（高度一致）。  

********************************
### **一棵m阶的B+树定义如下:**
1. 每个结点至多有m个子结点；  
2. 除根结点外，每个结点至少有⌈ m/2⌉个子结点，根结点至少有两个子结点；  
3. 有k个子结点的结点必有k个关键字。 

********************************
### **B+树的特征**
1. 有m个子树的内部节点包含有m个元素（B树中是k-1个元素），每个元素不保存数据，只用来索引。  
2. 所有的叶子结点中包含了全部关键字的信息，及指向含有这些关键字记录的指针，且叶子结点本身依关键字的大小自小而大的顺序链接。 (而B 树的叶子节点并没有包括全部需要查找的信息)。    
3. 所有的非叶子结点可以看成是索引部分，结点中仅含有其子树根结点中最大（或最小）关键字。 (而B 树的非叶子点也包含需要查找的有效信息)。  

********************************
### **聚集索引和非聚集索引**
在MySQL中，B+树索引按照存储方式的不同分为聚集索引和非聚集索引。这里介绍innodb中的聚集索引和非聚集索引。  

1. 聚集索引（聚簇索引）：以innodb作为存储引擎的表，表中的数据都会有一个主键，即使你不创建主键，系统也会帮你创建一个隐式的主键。这是因为innodb是把数据存放在B+树中的，而B+树的键值就是主键，在B+树的叶子节点中，存储了表中所有的数据。**这种以主键作为B+树索引的键值而构建的B+树索引，我们称之为聚集索引。**   

2. 非聚集索引（非聚簇索引）：**以主键以外的列值作为键值构建的B+树索引，我们称之为非聚集索引。** 非聚集索引与聚集索引的区别在于非聚集索引的叶子节点不存储表中的数据，而是存储该列对应的主键，想要查找数据我们还需要根据主键再去聚集索引中进行查找，这个再根据聚集索引查找数据的过程，我们称为回表。  

********************************
### 为什么说B+树比B树更适合数据库索引？
1. B+树的磁盘读写代价更低  
　　B+树的内部结点并没有指向关键字具体信息的指针。因此其内部结点相对B 树更小。如果把所有同一内部结点的关键字存放在同一盘块中，那么盘块所能容纳的关键字数量也越多。一次性读入内存中的需要查找的关键字也就越多。相对来说IO读写次数也就降低了。  

2. B+树查询效率更加稳定  
　　由于非终结点并不是最终指向文件内容的结点，而只是叶子结点中关键字的索引。所以任何关键字的查找必须走一条从根结点到叶子结点的路。所有关键字查询的路径长度相同，导致每一个数据的查询效率相当。  

3. B+树便于范围查询（最重要的原因，范围查找是数据库的常态）    
　　B树在提高了IO性能的同时并没有解决元素遍历的效率低下的问题，正是为了解决这个问题，B+树应用而生。B+树只需要去遍历叶子节点就可以实现整棵树的遍历。而且在数据库中基于范围的查询是非常频繁的。B树的范围查找用的是中序遍历，而B+树用的是在链表上遍历。  

