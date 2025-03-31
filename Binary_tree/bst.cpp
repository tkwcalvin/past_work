#include "bst.h"
using namespace std;

/**
 * Returns true if "src" contains *any* words in "filterWords" (case-insensitive).
 * e.g.: 
 *   containAnyWords("Example string", "example") == true
 *   containAnyWords("Example string", "amp") == true
 *   containAnyWords("Example string", "this is a long sentence") == true  [contains 'a']
 *   containAnyWords("Example string", "no occurence") == false
*/
bool containAnyWords(string src, string filterWords) {
    auto strToLower = [](string& str) {
        for (char& c: str) c = tolower(c);
    };
    strToLower(src);
    strToLower(filterWords);
    size_t found = filterWords.find(' ');
    while (found != string::npos) {
        if (src.find(filterWords.substr(0, found)) != string::npos)
            return true;
        filterWords = filterWords.substr(found+1);
        found = filterWords.find(' ');
    }
    return src.find(filterWords) != string::npos;
}


// Filter::match(const Animal&) const
// Returns true if the animal satisfies ALL 3 conditions:
// - species (including breed) contains *any* of the words in speciesMatch
// - health description contains *any* of the words in healthMatch
// - vaccine status contains *all* of the non-empty strings in vaccineMatch
bool Filter::match(const Animal& a) const
{
    
    if (!containAnyWords(a.getSpecies(),speciesFilter)){
        return false;
    }
    if (!containAnyWords(a.getHealthCondition().description,healthFilter)){
        return false;
    }
    for (unsigned int i=0; i<VACCINE_TABLE_SIZE; i++){
        if (!vaccineFilter[i].empty()){
            if (!a.getVaccinationStatus().hasVaccine(vaccineFilter[i])){
                return false;
            }
        }
    }
    return true;

}

// AnimalLLnode::print(unsigned int&, unsigned int&, const Filter&) const
// Print the animals in this linked list if it matches with the filter.
// The linked list should be maintained such that it is in decreasing ID order.
//
// E.g. (each node is shown as ID[Name]):
//      7[Cat] -> 6[Cat] -> 5[Dog] -> 3[Cat] -> 1[Bird] -> 0[Cat]
// should be printed in the following order using Filter{Name = "Cat"}: 
//      0[Cat]
//      3[Cat]
//      6[Cat]
//      7[Cat]
//
// The parameters ignoreCount and displayCount should be passed to the animal's display() function
void AnimalLLnode::print(unsigned int& ignoreCount, unsigned int& displayCount, const Filter& filter) const
{
    if (filter.match(*animal)){
        if (next!=nullptr){
            next->print(ignoreCount, displayCount, filter);
        }
        animal->display(ignoreCount, displayCount);
    }
    else{
        if (next!=nullptr){
            next->print(ignoreCount, displayCount, filter);
        }
    }

}

// BSTnode destructor
BSTnode::~BSTnode()  {
    if (head==nullptr){
        return;
    }
    AnimalLLnode *p=head, *q=head->next;
    while(q!=nullptr){
        delete p;
        p=q;
        q=q->next;
    }
    delete p;

}

// BSTnode::addAnimal(const Animal* a)
// Add an animal to the linked list.
// Ensure the order is *decreasing ID*, e.g.: 7[Cat] -> 6[Cat] -> 5[Dog] -> 3[Cat] -> 1[Bird] -> 0[Cat]
// You may assume no two animals with the same ID will be added to a node
// (this also means you should not add the same animal to a node twice!)
void BSTnode::addAnimal(const Animal* a) {
    if (head==nullptr){
        head=new AnimalLLnode(a, nullptr);
        return;
    }
    //cout<<"1"<<endl;
    AnimalLLnode *p=head;
    if (a->getID() > head->animal->getID()){
        head=new AnimalLLnode(a, p);
        return;
    }
    //cout<<"2"<<endl;
    while(p->next!=nullptr && a->getID() < p->next->animal->getID()){
        p=p->next;
    }
    //cout<<"3"<<endl;
    if (p->next==nullptr){
        p->next=new AnimalLLnode(a, nullptr);
        return;
    }
    //cout<<"4"<<endl;
    AnimalLLnode* tmp=p->next;
    p->next=new AnimalLLnode(a, tmp);
}

// BSTnode::addAnimal(const Animal* a)
// Remove an animal from the linked list.
// Ensure the order of the other animals are kept.
// If the animal does not exist, do nothing.
// If there are no animals left after removing, set head to nullptr.
void BSTnode::removeAnimal(const Animal* a) {
    
    if (head==nullptr){
        return;
    }
    AnimalLLnode *p=head, *q=nullptr;
    while(p!=nullptr && p->animal!=a){
        q=p;
        p=p->next;
    }
    if (p==nullptr){
        return;
    }
    if (q==nullptr){
        head=p->next;
        delete p;
        return;
    }
    q->next=p->next;
    p->next=nullptr;
    delete p;
    
}


//  BST destructor
BST::~BST() {
    delete root;

}

// BST::findMinNode()
// Optional task, but may be needed for BST::remove().
// Return a reference to the BSTnode* of the min node in this BST.
BSTnode*& BST::findMinNode()
{
    if (root->left.root==nullptr){return root;}
    else{
        return root->left.findMinNode();
    }

}

// BST::insert(const Animal* a)
// Insert an animal 'a' to the BST.
// Use the comparator "data member function" to compare the animal with the current node:
// - If 'a' is "less than" the current node, insert it to the left subtree.
// - If 'a' is "more than" the current node, insert it to the right subtree.
// - If 'a' "equals" the current node, insert it into this node's linked list.
// - Otherwise, if the node is empty, create a new node using 'a'.
void BST::insert(const Animal* a)
{
    if (root==nullptr){
        root=new BSTnode(a, comparator);
        return;
    }
    int cmp=comparator(a, root->head->animal);
    if (cmp<0){
        if (root->left.root==nullptr){
            root->left.root=new BSTnode(a, comparator);
            return;
        }
        root->left.insert(a);
    }
    else if (cmp==0){
        root->addAnimal(a);
        return;
    }
    else{
        if (root->right.root==nullptr){
            root->right.root=new BSTnode(a, comparator);
            return;
        }
        root->right.insert(a);
    }

}

// BST::remove(const Animal* a)
// Remove an animal 'a' from the BST
// Follow the same steps in BST::insert() to locate the node to remove.
// Removal strategy is similar to lecture notes example with a few differences:
// - If the node is not found, do nothing.
// - If the node is found, remove the animal from its linked list. If it makes the linked list become empty,
//   remove the node:
//   + If the node contains 0 or 1 child, move the children node to current root, and deallocate the old root.
//   + Else, *move* the linked list from the right subtree's min node to current root, and deallocate right subtree's min node.
void BST::remove(const Animal* a)
{
    if (root==nullptr){
        return;
    }
    int cmp=comparator(a, root->head->animal);
    if (cmp<0){
        root->left.remove(a);
    }
    else if (cmp==0){
        root->removeAnimal(a);
        if (root->head==nullptr){
            if (root->left.root==nullptr && root->right.root==nullptr){
                delete root;
                root=nullptr;
            }
            else if (root->left.root==nullptr){
                BSTnode* tmp=root;
                root=root->right.root;
                tmp->right.root=nullptr;
                delete tmp;
            }
            else if (root->right.root==nullptr){
                BSTnode* tmp=root;
                root=root->left.root;
                tmp->left.root=nullptr;
                delete tmp;
            }
            else{
                BSTnode*& minNode=root->right.findMinNode();
                root->head=minNode->head;
                minNode->head=nullptr;
                BSTnode* tmp = minNode;
                minNode=minNode->right.root;
                tmp->right.root=nullptr;
                delete tmp;
                tmp=nullptr;
            }
            
        }
        return;
    }
    else{
        root->right.remove(a);
        
    }

}

// BST::print(unsigned int&, unsigned int&, const Filter&) const
// Print the BST using in-order traversal.
//
// E.g.: Consider a BST containing animals sorted by species name:
//                      8[Reptile, Healthy] -> 6[Reptile, Bad]
//              5[Rabbit, Critical]
// 7[Dog, Healthy] -> 3[Dog, Healthy]
//                      9[Cat, Bad] -> 4[Cat, Healthy] -> 2[Cat, Very poor]
//              1[Bird, Healthy]
// 
// should print in the following order using Filter{Health = "Healthy"}:
//      1[Bird, Healthy]
//      4[Cat, Healthy]
//      3[Dog, Healthy]
//      7[Dog, Healthy]
//      8[Reptile, Healthy]
void BST::print(unsigned int& ignoreCount, unsigned int& displayCount, const Filter& filter) const
{
    if (root==nullptr){
        return;
    }
    if (root->left.root!=nullptr){
        root->left.print(ignoreCount, displayCount, filter);
    }

    root->head->print(ignoreCount,displayCount, filter);

    if (root->right.root!=nullptr){
        root->right.print(ignoreCount, displayCount, filter);
    }

}
