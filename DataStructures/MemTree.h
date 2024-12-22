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
    TreeNode *LeftNode;
    TreeNode *RightNode;
} TreeNode;

typedef struct MemTree
{
    TreeNode *Root;
} MemTree;

MemTree *CreateMemTree()
{
    MemTree *newTree = (MemTree *)malloc(sizeof(MemTree));
    MemLocation Location;
    Location.Start = 0;
    Location.End = 1023;
    newTree->Root = CreateNode(1024, Location);
    return newTree;
}

TreeNode *CreateNode(int Size, MemLocation Location)
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

MemLocation *TreeAllocate(MemTree *Tree, int Size)
{
    return RecursiveAllocate(Tree->Root, Size);
}

// DO NOT USE THIS FUNCTION MANUALLY USE THE ONE ABOVE
MemLocation *RecursiveAllocate(TreeNode *Node, int ProcessSize)
{
    MemLocation *AllocatedLocation = NULL;
    // Recursive calls
    if (Node->LeftNode)
        AllocatedLocation = RecursiveAllocate(Node->LeftNode, ProcessSize);

    if (AllocatedLocation)
        return AllocatedLocation;

    if (Node->RightNode)
        AllocatedLocation = RecursiveAllocate(Node->LeftNode, ProcessSize);

    if (Node->LeftNode || Node->RightNode)
        return AllocatedLocation;
    // Base case if no recursion possible (No branches)
    if (Node->Allocated == true)
        return NULL;

    if (ProcessSize > Node->Size)
        return NULL;

    if (ProcessSize <= (Node->Size) / 2)
    {
        MemLocation LeftLocation;
        LeftLocation.Start = Node->Location.Start;
        LeftLocation.End = Node->Location.End / 2;
        Node->LeftNode = CreateNode((Node->Size) / 2, LeftLocation);

        MemLocation RightLocation;
        RightLocation.Start = (Node->Location.End / 2) + 1;
        RightLocation.End = Node->Location.End;
        Node->RightNode = CreateNode((Node->Size) / 2, RightLocation);

        return RecursiveAllocate(Node->LeftNode, ProcessSize);
    }
    else
    {
        Node->Allocated = true;
        return &Node->Location;
    }
}

void AttemptCombine(TreeNode *Node)
{
    if (!Node->LeftNode->LeftNode && !Node->RightNode->RightNode && Node->LeftNode->Allocated == false && Node->RightNode->Allocated == false)
    {
        free(Node->LeftNode);
        free(Node->RightNode);
        Node->LeftNode = NULL;
        Node->RightNode = NULL;
    }
}

bool TreeFree(MemTree *Tree, MemLocation Location)
{
    return RecursiveFree(Tree->Root, Location);
}

bool RecursiveFree(TreeNode *Node, MemLocation Location)
{
    // Recursive calls
    bool LeftFound = false;
    if (Node->LeftNode)
        LeftFound = RecursiveFree(Node->LeftNode, Location);

    if (LeftFound)
    {
        Node->LeftNode->Allocated = false;
        AttemptCombine(Node);
        return true;
    }

    bool RightFound = false;
    if (Node->RightNode)
        RightFound = RecursiveFree(Node->RightNode, Location);

    if (RightFound)
    {
        Node->RightNode->Allocated = false;
        AttemptCombine(Node);
        return true;
    }

    if (Node->LeftNode || Node->RightNode)
    {
        return false;
    }

    // Without branches case
    if (Node->Location.Start = Location.Start)
        return true;
    else
        return false;
}