#include "../headers.h"

typedef struct MemLocation
{
    int Start;
    int End;
} MemLocation;

typedef struct TreeNode
{
    int Size;
    bool Allocated;
    MemLocation Location;
    struct TreeNode *LeftNode;
    struct TreeNode *RightNode;
} TreeNode;

typedef struct MemTree
{
    TreeNode *Root;
} MemTree;

TreeNode *CreateTreeNode(int Size, MemLocation Location)
{
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->Size = Size;
    newNode->Allocated = false;
    newNode->Location.Start = Location.Start;
    newNode->Location.End = Location.End;
    newNode->LeftNode = NULL;
    newNode->RightNode = NULL;
    return newNode;
}

MemTree *CreateMemTree()
{
    MemTree *newTree = (MemTree *)malloc(sizeof(MemTree));
    MemLocation Location;
    Location.Start = 0;
    Location.End = 1023;
    newTree->Root = CreateTreeNode(1024, Location);
    return newTree;
}

// DO NOT USE THIS FUNCTION DIRECTLY USE THE ONE BELOW
MemLocation *RecursiveAllocate(TreeNode *treeNode, int ProcessSize)
{
    MemLocation *AllocatedLocation = NULL;
    // Recursive calls
    if (treeNode->LeftNode)
        AllocatedLocation = RecursiveAllocate(treeNode->LeftNode, ProcessSize);

    if (AllocatedLocation)
        return AllocatedLocation;

    if (treeNode->RightNode)
        AllocatedLocation = RecursiveAllocate(treeNode->RightNode, ProcessSize);

    if (treeNode->LeftNode || treeNode->RightNode)
        return AllocatedLocation;
    // Base case if no recursion possible (No branches)
    if (treeNode->Allocated == true)
        return NULL;

    if (ProcessSize > treeNode->Size)
        return NULL;

    if (ProcessSize <= (treeNode->Size) / 2)
    {
        MemLocation LeftLocation;
        LeftLocation.Start = treeNode->Location.Start;
        LeftLocation.End = treeNode->Location.End / 2;
        treeNode->LeftNode = CreateTreeNode((treeNode->Size) / 2, LeftLocation);

        MemLocation RightLocation;
        RightLocation.Start = (treeNode->Location.End / 2) + 1;
        RightLocation.End = treeNode->Location.End;
        treeNode->RightNode = CreateTreeNode((treeNode->Size) / 2, RightLocation);

        return RecursiveAllocate(treeNode->LeftNode, ProcessSize);
    }
    else
    {
        treeNode->Allocated = true;
        return &treeNode->Location;
    }
}

MemLocation *TreeAllocate(MemTree *Tree, int Size)
{
    return RecursiveAllocate(Tree->Root, Size);
}

void AttemptCombine(TreeNode *treeNode)
{
    if (!treeNode->LeftNode->LeftNode && !treeNode->RightNode->RightNode && treeNode->LeftNode->Allocated == false && treeNode->RightNode->Allocated == false)
    {
        free(treeNode->LeftNode);
        free(treeNode->RightNode);
        treeNode->LeftNode = NULL;
        treeNode->RightNode = NULL;
    }
}

// DO NOT USE THIS FUNCTION DIRECTLY USE THE ONE BELOW
bool RecursiveFree(TreeNode *treeNode, int Location)
{
    // Recursive calls
    bool LeftFound = false;
    if (treeNode->LeftNode)
        LeftFound = RecursiveFree(treeNode->LeftNode, Location);

    if (LeftFound)
    {
        treeNode->LeftNode->Allocated = false;
        AttemptCombine(treeNode);
        return true;
    }

    bool RightFound = false;
    if (treeNode->RightNode)
        RightFound = RecursiveFree(treeNode->RightNode, Location);

    if (RightFound)
    {
        treeNode->RightNode->Allocated = false;
        AttemptCombine(treeNode);
        return true;
    }

    if (treeNode->LeftNode || treeNode->RightNode)
    {
        return false;
    }

    // Without branches case
    if (treeNode->Location.Start = Location)
        return true;
    else
        return false;
}

bool TreeFree(MemTree *Tree, int Location)
{
    return RecursiveFree(Tree->Root, Location);
}

void DeleteTree(MemTree *Tree)
{
    // TODO delete nodes
    free(Tree);
}