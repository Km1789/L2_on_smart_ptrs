#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <iterator>
#include <memory>

using namespace std;

//----------------------------
template <class T>
class List
{
    struct Node
    {
        T v;
        shared_ptr<Node> next;
        weak_ptr<Node> prev;

        Node() = default;

        Node(const T &x) : Node()
        {
            v = x;
        }
    };
    
    shared_ptr<Node> b, e, cur;

public:
    List()=default;

    List(initializer_list<T> l) : List()
    {
        for (auto &x : l)
        {
            AddAfter(x);
        }
    }

    ~List()
    {
        clear();
    }

    void clear()
    {
        while (!empty())
        {
            DelCur();
        }
    }

    bool empty()
    {
        return cur == nullptr;
    }

    void DelCur()
    {
        if (cur)
        {
            shared_ptr<Node> n = cur;
            if (n->prev.lock())
            {
                n->prev.lock()->next = n->next;
                cur = n->prev.lock();
            }
            else
            {
                cur = b = n->next;
            }
            if (n->next)
            {
                n->next->prev = n->prev;
                cur = n->next;
            }
            else
            {
                cur = e = n->prev.lock();
            }
        }
    }

    void AddAfter(const T &x)
    {
        shared_ptr<Node> n = make_shared<Node>(x);
        if (cur == nullptr)
        {
            b = e = cur = n;
        }
        else
        {
            n->prev = cur;
            n->next = cur->next;
            cur->next = n;
            if (n->next)
            {
                n->next->prev = n;
            }
            else
            {
                e = n;
            }
            cur = n;
        }
    }

    void AddBefore(const T &x)
    {
        shared_ptr<Node> n = make_shared<Node>(x);
        if (cur == nullptr)
        {
            b = e = cur = n;
        }
        else
        {
            n->next = cur; 
            n->prev = cur->prev;
            cur->prev = n;
            if (n->prev.lock())
            {
                n->prev.lock()->next = n;
            }
            else
            {
                b = n;
            }
            cur = n;
        }
    }

    bool GoNext()
    {
        if (cur->next)
        {
            cur = cur->next;
            return true;
        }
        return false;
    }

    bool GoPrev()
    {
        if (cur->prev.lock())
        {
            cur = cur->prev.lock();
            return true;
        }
        return false;
    }

    void GoToBegin()
    {
        cur = b;
    }

    void GoToEnd()
    {
        cur = e;
    }

    T &Get()
    {
        return cur->v;
    }

    //------
    struct iterator
    {
        List * l;
        shared_ptr<Node> n;
        iterator()
        {
            l = nullptr;
            n = nullptr;
        }

        iterator(List * l, shared_ptr<Node> n)
        {
            this->l = l;
            this->n = n;
        }

        bool operator!=(const iterator&b)const
        {
            return n != b.n;
        }

        void operator++()
        {
            n = n->next;
        }

        T& operator*()
        {
            return n->v;
        }
    };

    iterator begin()
    {
        return iterator(this, b);
    }

    iterator end()
    {
        return iterator(this, nullptr);
    }
    //------

    using value_type = T;

    iterator insert(iterator it, const T&x)
    {
        cur = it.n;
        AddBefore(x);
        return iterator(this, cur);
    }

    iterator push_front(const T&x)
    {
        cur = b;
        AddBefore(x);
        return iterator(this, cur);
    }

    iterator push_back(const T&x)
    {
        cur = e;
        AddAfter(x);
        return iterator(this, cur);
    }
};
//----------------------------

template<class L> struct Insert_iterator
{
    L * l;
    typename L::iterator it;
    Insert_iterator()
    {
        l = nullptr;
    }

    Insert_iterator(L &l, typename L::iterator it)
    {
        this->l = &l;
        this->it = it;
    }

    void operator=(typename L::value_type &x)
    {
        l->insert(it, x);
    }
};

template<class L> Insert_iterator<L> Inserter(L &l, typename L::iterator it)
{
    return Insert_iterator(l, it);
}

//----------------------------
int main()
{
    {
        cout << "-- List forward --" << endl;
        List<int> l = {1, 2, 3, 4, 5};
        l.GoToBegin();
        if (!l.empty())
        {
            do
            {
                cout << l.Get() << " ";
            } while (l.GoNext());
            cout << endl;
        }
    }

    {
        cout << "-- List backward --" << endl;
        List<int> l = {1, 2, 3, 4, 5};
        l.GoToEnd();
        if (!l.empty())
        {
            do
            {
                cout << l.Get() << " ";
            } while (l.GoPrev());
            cout << endl;
        }
    }

    {
        cout << "-- List forward by iterator --" << endl;
        List<int> l = {1, 2, 3, 4, 5};
        for (List<int>::iterator it = l.begin(); it != l.end(); ++it)
        {
            cout << *it << " "; 
        }
        cout << endl;
    }

    {
        cout << "-- List forward by ranged-based for --" << endl;
        List<int> l = {1, 2, 3, 4, 5};
        for (auto &x : l)
        {
            cout << x << " "; 
        }
        cout << endl;
    }

    {
        cout << "-- list forward by iterator --" << endl;
        list<int> l = {1, 2, 3, 4, 5};
        for (list<int>::iterator it = l.begin(); it != l.end(); ++it)
        {
            cout << *it << " "; 
        }
        cout << endl;
    }

    {
        cout << "-- list forward by ranged-based for --" << endl;
        list<int> l = {1, 2, 3, 4, 5};
        for (auto &x : l)
        {
            cout << x << " "; 
        }
        cout << endl;
    }

    {
        cout << "-- list backward by iterator --" << endl;
        list<int> l = {1, 2, 3, 4, 5};
        for (list<int>::reverse_iterator it = l.rbegin(); it != l.rend(); ++it)
        {
            cout << *it << " "; 
        }
        cout << endl;
    }

    {
        cout << "-- list forward by insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;
        for (auto &x: v)
        {
            insert_iterator<list<int>> it(l, l.begin());
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- List forward by insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (auto &x: v)
        {
            Insert_iterator<List<int>> it(l, l.begin());
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- list forward by inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;    
        for (auto &x: v)
        {
            inserter<list<int>>(l, l.begin()) = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- List forward by Inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (auto &x: v)
        {
            Inserter<List<int>>(l, l.begin()) = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- list forward by front_insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;
        for (auto &x: v)
        {
            front_insert_iterator<list<int>> it(l);
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- list forward by back_insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;
        for (auto &x: v)
        {
            back_insert_iterator<list<int>> it(l);
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- list forward by front_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;    
        for (auto &x: v)
        {
            front_inserter<list<int>>(l) = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- list forward by back_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        list<int> l;    
        for (auto &x: v)
        {
            back_inserter<list<int>>(l) = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    cout << "---------------------" << endl;
    {
        cout << "-- List forward by front_insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (auto &x: v)
        {
            front_insert_iterator<List<int>> it(l);
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }    {
        cout << "-- List forward by back_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (int i = 0; i < 10000; i++)
        {
            back_inserter<List<int>>(l) = i;
        }
        for (auto &x: v)
        {
            back_inserter<List<int>>(l) = x;
        }
        // for (auto &x : l)
        // {
        //     cout << x << " ";
        // }
        // cout << endl;
    }
    cout << "Done" << endl;
        cout << endl;    {
        cout << "-- List forward by back_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (int i = 0; i < 10000; i++)
        {
            back_inserter<List<int>>(l) = i;
        }
        for (auto &x: v)
        {
            back_inserter<List<int>>(l) = x;
        }
        // for (auto &x : l)
        // {
        //     cout << x << " ";
        // }
        // cout << endl;
    }
    cout << "Done" << endl;
    }

    {
        cout << "-- List forward by back_insert_iterator --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (auto &x: v)
        {
            back_insert_iterator<List<int>> it(l);
            it = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- List forward by front_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;    
        for (auto &x: v)
        {
            front_inserter<List<int>>(l) = x;
        }
        for (auto &x : l)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    {
        cout << "-- List forward by back_inserter --" << endl;
        vector<int> v = {1, 2, 3, 4, 5};
        List<int> l;
        for (int i = 0; i < 10000; i++)
        {
            back_inserter<List<int>>(l) = i;
        }
        for (auto &x: v)
        {
            back_inserter<List<int>>(l) = x;
        }
        // for (auto &x : l)
        // {
        //     cout << x << " ";
        // }
        // cout << endl;
    }
    cout << "Done" << endl;
    return 0;
}