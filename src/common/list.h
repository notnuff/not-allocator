#ifndef LIST_H
#define LIST_H

template<class T>
class ListNode {
public:
  T* current_obj_ptr_ = nullptr;
  ListNode<T>* next_node_ = nullptr;
};

struct ListNodeT {
  ListNodeT* next_node_ = nullptr;
};

template<class T>
class DoubleLinkedListNode {
public:
  static void AppendBeforeNode(DoubleLinkedListNode<T> node, T* obj);
  static void AppendAfterNode(DoubleLinkedListNode<T> node, T* obj);
  static void RemoveFromListToList(T* head);

public:
  T* prev_node_ = nullptr;
  T* next_node_ = nullptr;
};

#endif //LIST_H
