#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <stack>
#include <random>

using namespace std;

class Record
{
    public:
        long long id;
        string f_name, l_name;
        string email;
        long long adr_id;

        Record(long long id, string f_name, string l_name, string email, long adr_id)
        {
            this->id = id;
            this->f_name = f_name;
            this->l_name = l_name;
            this->email = email;
            this->adr_id = adr_id;
        }

        Record(string line)
        {
            int position = 0;
            vector<int> positions;
            string delimiter = "|";
            while ((position = line.find(delimiter, position)) != std::string::npos) 
            {
                positions.push_back(position);
                position += delimiter.length();
            }
            this->id = stoll(line.substr(0, positions[0]));
            this->f_name = line.substr(positions[0] + 1, positions[1] - positions[0] - 1);
            this->l_name = line.substr(positions[1] + 1, positions[2] - positions[1] - 1);
            this->email = line.substr(positions[2] + 1, positions[3] - positions[2] - 1);
            this->adr_id = stoll(line.substr(positions[3] + 1, line.length() - positions[3] - 1));
        }

        Record(Record *other)
        {
            this->id = other->id;
            this->f_name = other->f_name;
            this->l_name = other->l_name;
            this->email = other->email;
            this->adr_id = other->adr_id;
        }

        void printOneLine()
        {
            cout << this->id << "|" << this->f_name << "|" << this->l_name << "|" << this->email << "|" << this->adr_id << endl;
        }

        void print()
        {
            cout << "ID: " << this->id << endl;
            // cout << "First Name: " << this->f_name << endl;
            // cout << "Last Name: " << this->l_name << endl;
            // cout << "Email: " << this->email << endl;
            // cout << "Address ID: " << this->adr_id << endl;
        }

        bool operator==(const Record &other) const
        {
            return this->id == other.id;
        }

        bool operator<(const Record &other) const
        {
            return this->id < other.id;
        }

        bool operator>(const Record &other) const
        {
            return this->id > other.id;
        }

        bool operator<=(const Record &other) const
        {
            return this->id <= other.id;
        }

        bool operator>=(const Record &other) const
        {
            return this->id >= other.id;
        }
};

class FileReader
{
    public:
        vector<Record*> records;

        FileReader(string file_name)
        {
            ifstream file(file_name);
            string line;

            while (file >> line)
            {
                Record *record = new Record(line);
                records.push_back(record);
            }
        }
};

class FileWriter
{
    public:
        ofstream *fajl;

        FileWriter(string filename)
        {
            fajl = new ofstream(filename);
        }

        ~FileWriter()
        {
            fajl->close();
            delete fajl;
        }

        void writeRecords(vector<Record*> records)
        {
            for (int i = 0; i < records.size(); i++)
            {
                if (records[i] != NULL)
                    *fajl << records[i]->id << "|" << records[i]->f_name << "|" << records[i]->l_name << "|" << records[i]->email << "|" << records[i]->adr_id << endl;
                else
                    *fajl << "NULL" << endl;
            }
        }
};

class BStarNode
{
    public:
        vector<Record*> keys;
        vector<BStarNode*> children;
        int m;
        bool is_root;
        // if is_root then parent is NULL
        BStarNode* parent;

    BStarNode(vector<Record*> record, int m, BStarNode *parent = NULL)
    {
        keys = record;

        for (int i = 0; i < keys.size()+1; i++)
            children.push_back(NULL);
        
        this->m = m;
        this->is_root = (parent == NULL);
        this->parent = parent;
    }

    ~BStarNode()
    {
        for (int i = 0; i < keys.size(); i++)
            delete keys[i];
    }

    void print()
    {
        for (int i = 0; i < keys.size(); i++)
        {
            keys[i]->print();
            cout << endl;
        }
        for (int i = 0; i < children.size(); i++)
        {
            if (children[i] == NULL)
                cout << "NULL, ";
            else
                cout << i << ", ";
        }
        cout << endl << endl;
    }

    int findKey(Record *record)
    {
        int pos = 0;

        while (pos < keys.size() && *record > *keys[pos])
        {
            pos++;
        }

        return pos;        
    }

    bool addKeyToNode(Record *record)
    {
        int insert_pos = findKey(record);

        if (insert_pos == keys.size())
        {
            keys.push_back(record);
            children.push_back(NULL);
        }
        else
        {
            keys.insert(keys.begin() + insert_pos, record);
            children.insert(children.begin() + insert_pos, NULL);
        }
        return children.size() <= m;
    }

    bool overflowInsert()
    {
        if (children.size() <= m)
            return true;

        BStarNode* up = this->parent;
        Record *pom = this->keys[0];
        int pos = up->findKey(pom);

        

        if (pos != 0 && up->children[pos-1]->children.size() < m)
        { 
            BStarNode *left = up->children[pos-1];
            int added_size = left->keys.size() + 1 + this->keys.size();
            int break_indx = added_size/2 - left->keys.size() - 1;

            vector<Record*> extra_keys;
            vector<BStarNode*> extra_children;
            Record* break_key = this->keys[break_indx];

            for (int i = 0; i < break_indx; i++)
            {
                extra_keys.push_back(this->keys[i]);
                extra_children.push_back(this->children[i]);
            }
            extra_children.push_back(this->children[break_indx]);
            this->keys.erase(keys.begin(), keys.begin() + break_indx+1);
            this->children.erase(children.begin(), children.begin() + break_indx+1);

            left->keys.push_back(up->keys[pos-1]);
            for (int i = 0; i < extra_keys.size(); i++)
                left->keys.push_back(extra_keys[i]);
            for (int i = 0; i < extra_children.size(); i++)
                left->children.push_back(extra_children[i]);

            for (int i = 0; i < extra_children.size(); i++)
                if (extra_children[i] != NULL)
                    extra_children[i]->parent = left;

            up->keys[pos-1] = break_key;
            return true;
        }
        if (pos != up->keys.size() && up->children[pos+1]->children.size() < m)
        {
            BStarNode *right = up->children[pos+1];
            int added_size = right->keys.size() + 1 + this->keys.size();
            int break_indx = added_size/2;

            vector<Record*> extra_keys;
            vector<BStarNode*> extra_children;
            Record* break_key = this->keys[break_indx];

            for (int i = break_indx+1; i < this->keys.size(); i++)
            {
                extra_keys.push_back(this->keys[i]);
                extra_children.push_back(this->children[i]);
            }
            extra_children.push_back(this->children[children.size()-1]);
            this->keys.resize(break_indx);
            this->children.resize(break_indx+1);
            
            extra_keys.push_back(up->keys[pos]);
            for (int i = 0; i < right->keys.size(); i++)
                extra_keys.push_back(right->keys[i]);
            for (int i = 0; i < right->children.size(); i++)
                extra_children.push_back(right->children[i]);

            for (int i = 0; i < extra_children.size(); i++)
                if (extra_children[i] != NULL)
                    extra_children[i]->parent = this;

            right->children = extra_children;
            right->keys = extra_keys;

            up->keys[pos] = break_key;
            return true;
        }
        return false;
    }

    bool breakInsert()
    {
        BStarNode* up = this->parent;
        Record *pom = this->keys[0];
        int pos = up->findKey(pom);

        if (pos != up->keys.size())
        {
            BStarNode* right = up->children[pos+1];
            // prvi 2m-2 /3 drugi 2m-1 /3 treci 2m/3

            int full_size = 2*m;
            int break_point1 = (2*m - 2)/3; // u left
            int break_point2 = break_point1 + (2*m - 1)/3  - m; // u right

            vector<Record*> mid_keys;
            vector<BStarNode*> mid_chlrn;

            Record *bp1_key = this->keys[break_point1];
            Record *bp2_key = right->keys[break_point2];


            for (int i = break_point1+1; i < this->keys.size(); i++)
            {
                mid_keys.push_back(this->keys[i]);
                mid_chlrn.push_back(this->children[i]);
            }
            mid_chlrn.push_back(this->children[children.size()-1]);

            this->keys.resize(break_point1);
            this->children.resize(break_point1+1);

            mid_keys.push_back(up->keys[pos]);

            for (int i = 0; i < break_point2; i++)
            {
                mid_keys.push_back(right->keys[i]);
                mid_chlrn.push_back(right->children[i]);
            }
            mid_chlrn.push_back(right->children[break_point2]);

            right->keys.erase(right->keys.begin(), right->keys.begin() + break_point2 + 1);
            right->children.erase(right->children.begin(), right->children.begin() + break_point2 + 1);

            BStarNode *new_node = new BStarNode(mid_keys, m, up);
            new_node->children = mid_chlrn;

            for (int i = 0; i < mid_chlrn.size(); i++)
                if (mid_chlrn[i] != NULL)
                    mid_chlrn[i]->parent = new_node;

            up->keys[pos] = bp1_key;
            up->keys.insert(up->keys.begin() + pos+1, bp2_key);
            up->children.insert(up->children.begin() + pos+1, new_node);
            
            return up->children.size() <= m;
        }
        else
        {   // reminder da je izmedju left i this keys[pos-1]
            BStarNode* left = up->children[pos-1];
            // prvi 2m-2 /3 drugi 2m-1 /3 treci 2m/3

            int full_size = 2*m;
            int break_point1 = (2*m - 2)/3; // u left
            int break_point2 = break_point1 + 1 + (2*m - 1)/3  - m; // u right

            vector<Record*> mid_keys;
            vector<BStarNode*> mid_chlrn;

            Record *bp1_key = left->keys[break_point1];
            Record *bp2_key = this->keys[break_point2];


            for (int i = break_point1+1; i < left->keys.size(); i++)
            {
                mid_keys.push_back(left->keys[i]);
                mid_chlrn.push_back(left->children[i]);
            }
            mid_chlrn.push_back(left->children[left->children.size()-1]);

            left->keys.resize(break_point1);
            left->children.resize(break_point1+1);

            mid_keys.push_back(up->keys[pos-1]);

            for (int i = 0; i < break_point2; i++)
            {
                mid_keys.push_back(this->keys[i]);
                mid_chlrn.push_back(this->children[i]);
            }
            mid_chlrn.push_back(children[break_point2]);

            this->keys.erase(keys.begin(), keys.begin() + break_point2 + 1);
            this->children.erase(children.begin(), children.begin() + break_point2 + 1);

            BStarNode *new_node = new BStarNode(mid_keys, m, up);
            new_node->children = mid_chlrn;

            for (int i = 0; i < mid_chlrn.size(); i++)
                if (mid_chlrn[i] != NULL)
                    mid_chlrn[i]->parent = new_node;

            up->keys[pos-1] = bp1_key;
            up->keys.insert(up->keys.begin() + pos, bp2_key);
            up->children.insert(up->children.begin() + pos, new_node);

            return up->children.size() <= m;
        }
    }

    void expandRoot()
    {
        // to do
        if (children.size() < 2*((2*m - 2)/3)+1)
            return;
        int br_p1 = keys.size()/2;
        
        vector<Record*> a, b;
        vector<BStarNode*> a_chld, b_chld;
        Record *key1 = keys[br_p1];

        for (int i = 0; i < br_p1; i++)
            a.push_back(keys[i]);
        for (int i = br_p1+1; i < keys.size(); i++)
            b.push_back(keys[i]);

        for (int i = 0; i <= br_p1; i++)
            a_chld.push_back(children[i]);
        for (int i = br_p1+1; i < children.size(); i++)
            b_chld.push_back(children[i]);

        

        BStarNode *tree_a = new BStarNode(a, m, this);
        BStarNode *tree_b = new BStarNode(b, m, this);
        for (int i = 0; i < a_chld.size(); i++)
            if (a_chld[i] != NULL)
                a_chld[i]->parent = tree_a;

        for (int i = 0; i < b_chld.size(); i++)
            if (b_chld[i] != NULL)
                b_chld[i]->parent = tree_b;

        tree_a->children = a_chld;
        tree_b->children = b_chld;
        
        vector<Record*> new_keys = {key1};
        vector<BStarNode*> new_children = {tree_a, tree_b};

        keys = new_keys;
        children = new_children;
    }

    BStarNode* deleteKeyFromNode(Record *record)
    {
        BStarNode *pom = deleteByFollower(record);
        if (pom != NULL)
            return pom;

        int pos = this->findKey(record);

        keys.erase(keys.begin() + pos);

        children.erase(children.begin() + pos + 1);
        return this;
    }

    void sendOneKeyToLeft()
    {
        Record *pom = this->keys[0];
        int pos = parent->findKey(pom);
        BStarNode* left = parent->children[pos-1];

        Record *first_record = this->keys[0];
        BStarNode *first_child = this->children[0];

        this->keys.erase(keys.begin());
        this->children.erase(children.begin());

        left->keys.push_back(parent->keys[pos-1]);
        left->children.push_back(first_child);

        parent->keys[pos-1] = first_record;

        if (first_child != NULL)
            first_child->parent = left;       
    }

    void sendOneKeyToRight()
    {
        Record *pom = this->keys[0];
        int pos = parent->findKey(pom);
        BStarNode* right = parent->children[pos+1];

        Record *last_record = this->keys[keys.size()-1];
        BStarNode *last_child = this->children[children.size()-1];

        this->keys.pop_back();
        this->children.pop_back();

        right->keys.insert(right->keys.begin(), parent->keys[pos]);
        right->children.insert(right->children.begin(), last_child);

        parent->keys[pos] = last_record;

        if (last_child != NULL)
            last_child->parent = right;
    }

    bool overflowDelete()
    {
        BStarNode* up = this->parent;
        Record *pom = this->keys[0];
        int pos = up->findKey(pom);

        if (pos != 0 && (up->children[pos-1]->children.size() > (2*m -1)/3 
                || (pos > 1 && up->children[pos-2]->children.size() > (2*m -1)/3) ))
        {
            up->children[pos-1]->sendOneKeyToRight();
            if (pos > 1 && up->children[pos-1]->children.size() < (2*m -1)/3)
                up->children[pos-2]->sendOneKeyToRight();
            return true;                
        }
        if (pos != up->keys.size() && (up->children[pos+1]->children.size() > (2*m -1)/3
        || (pos < up->keys.size()-1 && up->children[pos+2]->children.size() > (2*m-1)/3)))
        {
            up->children[pos+1]->sendOneKeyToLeft();
            if (pos < up->keys.size()-1 && up->children[pos+1]->children.size() < (2*m -1)/3)
                up->children[pos+2]->sendOneKeyToLeft();
                return true;
        }
        return false;
    }

    void mergeLeftRight()
    {
        Record *pom = this->keys[0];
        int pos = parent->findKey(pom);
        BStarNode *left = parent->children[pos-1];
        BStarNode *right = parent->children[pos+1];

        int added_size = left->keys.size() + 1 + this->keys.size() + 1 + right->keys.size();

        int break_indx = added_size/2 - left->keys.size() - 1;
        Record *break_key = this->keys[break_indx];

        vector<Record*> extra_keys;
        vector<BStarNode*> extra_children;

        for (int i = 0; i < break_indx; i++)
        {
            extra_keys.push_back(this->keys[i]);
            extra_children.push_back(this->children[i]);
        }
        extra_children.push_back(this->children[break_indx]);

        this->keys.erase(keys.begin(), keys.begin() + break_indx+1);
        this->children.erase(children.begin(), children.begin() + break_indx+1);

        left->keys.push_back(parent->keys[pos-1]);
        for (int i = 0; i < extra_keys.size(); i++)
            left->keys.push_back(extra_keys[i]);
        for (int i = 0; i < extra_children.size(); i++)
            left->children.push_back(extra_children[i]);

        for (int i = 0; i < extra_children.size(); i++)
            if (extra_children[i] != NULL)
                extra_children[i]->parent = left;

        parent->keys[pos-1] = break_key;

        this->keys.push_back(parent->keys[pos]);
        for (int i = 0; i < right->keys.size(); i++)
            this->keys.push_back(right->keys[i]);
        for (int i = 0; i < right->children.size(); i++)
            this->children.push_back(right->children[i]);

        for (int i = 0; i < right->children.size(); i++)
            if (right->children[i] != NULL)
                right->children[i]->parent = this;

        
        parent->keys.erase(parent->keys.begin() + pos);

        parent->children.erase(parent->children.begin() + pos+1);
    }

    void mergeIntoRoot()
    {
        vector<Record*> new_keys;
        vector<BStarNode*> new_children;

        for (int i = 0; i < this->children[0]->keys.size(); i++)
            new_keys.push_back(children[0]->keys[i]);
        for (int i = 0; i < this->children[0]->children.size(); i++)
            new_children.push_back(children[0]->children[i]);
        
        new_keys.push_back(keys[0]);

        for (int i = 0; i < this->children[1]->keys.size(); i++)
            new_keys.push_back(children[1]->keys[i]);

        for (int i = 0; i < this->children[1]->children.size(); i++)
            new_children.push_back(children[1]->children[i]);

        for (int i = 0; i < new_children.size(); i++)
            if (new_children[i] != NULL)
                new_children[i]->parent = this;

        keys = new_keys;
        children = new_children;    
    }

    bool mergeDelete()
    {
        BStarNode* up = this->parent;
        Record *pom = this->keys[0];
        int pos = up->findKey(pom);

        if (up->children.size() == 2)
        {
            up->mergeIntoRoot();
            return true;
        }

        if (pos == 0)
        {
            this->parent->children[pos+1]->mergeLeftRight();
            return true;
        }
        if (pos == up->keys.size())
        {
            up->children[pos-1]->mergeLeftRight();
            return true;
        }
        mergeLeftRight();
        return false;
    }

    BStarNode* deleteByFollower(Record *record)
    {
        if (this->children[0] == NULL)
            return NULL;
        int pos = findKey(record);

        BStarNode* curr = children[pos+1];
        while (curr->children[0] != NULL)
            curr = curr->children[0];

        Record *sledbenik = curr->keys[0];
        curr->keys.erase(curr->keys.begin());
        curr->children.erase(curr->children.begin());

        if (curr != this)   
            keys[pos] = sledbenik;
        
        return curr;
    }

    bool isValid()
    {
        return children.size() >= (2*m - 1)/3 && children.size() <= m;
    }
};

class BStarTree
{
    public:
        BStarNode *root;
        int m;
        string last_action;
        Record *last_added;

        BStarTree(int m)
        {
            this->m = m;
            root = new BStarNode({}, m);
        }

        BStarTree(int m, string filename)
        {
            this->m = m;
            FileReader *reader = new FileReader(filename);
            root = new BStarNode({}, m);
            for (int i = 0; i < reader->records.size(); i++)
                insertKey(reader->records[i]);

            cout << "Napravljeno stablo reda m = " << m << ", sa podacim iz fajla: " << filename << endl;
        }

        ~BStarTree()
        {
            deque<BStarNode*> q;
            BStarNode *curr;
            q.push_back(root);

            while (!q.empty())
            {
                curr = q.front(), q.pop_front();

                for (int i = 0; i < curr->children.size(); i++)
                    if (curr->children[i] != NULL)
                        q.push_back(curr->children[i]);

                delete curr;
            }
        }

        void insertKey(Record *record)
        {
            BStarNode *curr = root, *parent = NULL;
            last_added = record;

            while (curr != NULL)
            {
                int next_pos = curr->findKey(record);
                parent = curr;
                curr = curr->children[next_pos];
            }
            curr = parent;
            last_action = "normal inserting...";
            if (curr->addKeyToNode(record))
                return;
            
            while (true)
            {
                if (curr == root)
                {
                    last_action = "root expanding...";
                    curr->expandRoot();
                    break;
                }
                
                last_action = "overflow inserting...";
                if (curr->overflowInsert())
                    break;
                    
                last_action = "break inserting...";
                if (curr->breakInsert())
                    break;
                curr = curr->parent;
            }
        }

        BStarNode* findNode(Record *record, int *steps = NULL)
        {
            BStarNode *curr = root;

            while (curr != NULL)
            {
                int pos = curr->findKey(record);
                if (record->id == curr->keys[pos]->id)
                    break;
                curr = curr->children[pos];
                if (steps != NULL)
                    *steps += 1;
            }
            return curr;
        }

        bool deleteKey(Record *record)
        {
            BStarNode *curr = root, *pom;
            
            curr = findNode(record);

            if (curr == NULL)
                return false;
            

            // deleteKeyFromNode ce vratiti cvor iz kog je obrisao
            curr = curr->deleteKeyFromNode(record);
            last_action = "deleted from node directly, or by follower";
            if (curr->isValid())
                return true;

            while (curr != NULL)
            {
                last_action = "overflow deleting...";
                if (curr->overflowDelete())
                    break;
                last_action = "merge deleting...";
                curr->mergeDelete();
                curr = curr->parent;

                if (curr->isValid() || curr->is_root)
                    break;
            }
            
            return true;
        }

        void findError()
        {
            deque<BStarNode*> q;
            BStarNode *curr;
            q.push_back(root);

            while (!q.empty())
            {
                curr = q.front(), q.pop_front();

                bool foundNULL = false, foundNotNULL = false;

                for (int i = 0; i < curr->children.size(); i++)
                {
                    if (curr->children[i] == NULL)
                        foundNULL = true;
                    else
                        foundNotNULL = true;
                }

                if (foundNotNULL && foundNULL)
                {
                    cout << "parent of error: " << endl;
                    curr->parent->print();
                    cout << "node of error: " << endl;
                    curr->print();  
                    cout << "last inserted record: ";
                    last_added->print();
                    cout << "last_excecuted_action: " << last_action << endl;
                    exit(69);

                }

                for (int i = 0; i < curr->children.size(); i++)
                    if (curr->children[i] != NULL)
                        q.push_back(curr->children[i]);
            }
        }

        void print()
        {
            deque<BStarNode*> q;
            BStarNode *curr;
            q.push_back(root);

            while (!q.empty())
            {
                curr = q.front(), q.pop_front();

                curr->print();

                for (int i = 0; i < curr->children.size(); i++)
                    if (curr->children[i] != NULL)
                        q.push_back(curr->children[i]);
            }
        }  

        void parentChecking()
        {
            deque<BStarNode*> q;
            BStarNode *curr;
            q.push_back(root);
                
            int contr = 0;

            while (!q.empty())
            {
                curr = q.front(), q.pop_front();

                for (int i = 0; i < curr->children.size(); i++)
                    if (curr->children[i] != NULL)
                    {
                        q.push_back(curr->children[i]);
                        if (curr != curr->children[i]->parent)
                        {
                            contr++;
                        }
                    }
            }

            if (contr != 0)
            {
                cout << "number: " << contr << endl;
                cout << "parent error!!" << endl;
                cout << "after action: " << last_action << endl;
                cout << "last added: ";
                last_added->print(); 
            }
        }

        vector<Record*> inorderTraversal()
        {
            vector<Record*> records;
            stack<BStarNode*> stek;
            stack<int> indeksi;
            BStarNode *curr = root;

            while (curr != NULL)
            {
                stek.push(curr);
                indeksi.push(0);
                curr = curr->children[0];
            }

            while (!stek.empty())
            {
                curr = stek.top(), stek.pop();
                int pos = indeksi.top();
                indeksi.pop();


                if (curr->children[0] == NULL)
                {
                    for (int i = 0; i < curr->keys.size(); i++)
                        records.push_back(curr->keys[i]);
                }
                else
                {
                    if (pos < curr->keys.size())
                    {
                        records.push_back(curr->keys[pos]);
                        stek.push(curr);
                        indeksi.push(pos+1);
                    }
                    if (pos+1 < curr->children.size())
                    {
                        curr = curr->children[pos+1];
                        while (curr != NULL)
                        {
                            stek.push(curr);
                            indeksi.push(0);
                            curr = curr->children[0];
                        }
                    }
                }

                
            }

            return records;
        }

        void printInorder()
        {

            vector<Record*> records = inorderTraversal();
            for (int i = 0; i < records.size(); i++)
                records[i]->print();
        }

        void checkInorder()
        {
            vector<Record*> records = inorderTraversal();
            for (int i = 0; i < records.size()-1; i++)
                if (*records[i] > *records[i+1])
                {
                    cout << "inorder error!!" << endl;
                    cout << "after action: " << last_action << endl;
                    cout << "last added: ";
                    last_added->print();
                    cout << "error on index: " << i << endl;
                    records[i]->print();
                    records[i+1]->print();
                    exit(69);
                }
        }

        Record* findRecordById(long long id, int &steps, bool printR = false)
        {
            Record *temp_record = new Record(id, "a", "a", "a", 1);
            steps = 0;
            BStarNode *location = findNode(temp_record, &steps);
            if (location == NULL)
            {
                steps = 0;
                if (printR)
                    cout << "Nije nadjen red sa id-om: " << id << endl;
                return NULL;
            }   
            int pos = location->findKey(temp_record);

            if (printR)
            {
                cout << "Broj koraka: " << steps << endl;
                location->keys[pos]->printOneLine();
            }
            return location->keys[pos];
        }

        void findRecordsByName(string fname)
        {
            vector<Record*> matching_fnames;
            vector<Record*> inorder = inorderTraversal();

            for (int i = 0; i < inorder.size(); i++)
                if (fname == inorder[i]->f_name)
                    matching_fnames.push_back(inorder[i]);

            cout << "Broj koraka: "<< inorder.size() << endl;
            for (int i = 0; i < matching_fnames.size(); i++)
                matching_fnames[i]->printOneLine();

            if (matching_fnames.size() == 0)
                cout << "Nema redova sa imenom: " << fname << endl;
        }

        void findRecordsByIds(vector<long long> ids)
        {
            vector<Record*> found_records;
            int broj_koraka = 0;
            
            for (int i = 0; i < ids.size(); i++)
            {
                int steps;
                found_records.push_back(findRecordById(ids[i], steps));
                broj_koraka += steps;
            }
            cout << "Broj koraka: " << broj_koraka << endl;
            
            time_t now = time(0);
            tm *ltm = localtime(&now);
            string filename = "records_" + to_string(1900 + ltm->tm_year).substr(2) + "_" + to_string(1 + ltm->tm_mon) + "_" + to_string(ltm->tm_mday) + "_" + to_string(ltm->tm_hour) + "_" + to_string(ltm->tm_min) + "_" + to_string(ltm->tm_sec) + ".txt";
            
            FileWriter *pisi = new FileWriter(filename);
            pisi->writeRecords(found_records);

            delete pisi;
        }

        bool deleteById(long long id)
        {
            Record *temp_record = new Record(id, "a", "a", "a", 1);
            return deleteKey(temp_record);
        }

};



void testFunctionSmall(vector<Record*> records)
{
    BStarTree *drvo = new BStarTree(6);
    for (int i = 0; i < records.size(); i++)
    {
        drvo->insertKey(records[i]);
        drvo->parentChecking();
        drvo->findError();
        drvo->checkInorder();
    }

    vector<int> delRec;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 999);

    
    while (delRec.size() < 50) {
        delRec.push_back(dis(gen));
    }

    for (int i = 0; i < delRec.size(); i++)
    {
        if (i < records.size())
            drvo->deleteKey(records[delRec[i]]);
        drvo->parentChecking();
        drvo->findError();
        drvo->checkInorder();
    }
    

}

void testFunctionBig(vector<Record*> records, int repeat)
{
    for (int i = 0; i < repeat; i++)
    {
        testFunctionSmall(records);
        cout << "test no: " << i+1 << " done" << endl;
    }
    
}


void printMenu()
{
    cout << "1. Kreiraj index na osnovu fajla" << endl;
    cout << "2. Dodaj red" << endl;
    cout << "3. Pronadji red po ID-u" << endl;
    cout << "4. Pronadji red po imenu" << endl;
    cout << "5. Obrisi red po ID-u" << endl;
    cout << "6. Pronadji red po vise ID-eva" << endl;
    cout << "7. Ispisi ceo index" << endl;
    cout << "8. Obrisi index" << endl;
    cout << "9. Izadji" << endl;

    cout << endl << "--------------------------------" << endl << endl;
}

int main() 
{
    BStarTree *stablo = NULL;
    long long id, adresa;
    string ime, prezime, email;
    int izbor;
    vector<long long> ids;
    int num_ids;
    string filename;
    int m;

    while (true)
    {
        printMenu();
        cout << "Unesite izbor: ";
        cin >> izbor;

        if (izbor != 1 && izbor != 9 && stablo == NULL)
        {
            cout << "Nije kreiran index!" << endl;
            continue;
        }


        switch (izbor)
        {
            case (1) :
                cout << "Unesite ime fajla: ";
                cin >> filename;
                cout << "Unesite red stabla: ";
                cin >> m;
                stablo = new BStarTree(m, filename);
                cout << "Uspesno kreiran index!" << endl;
                break;
            case (2) :
                cout << "Unesite ID: ";
                cin >> id;
                cout << "Unesite ime: ";
                cin >> ime;
                cout << "Unesite prezime: ";
                cin >> prezime;
                cout << "Unesite email: ";
                cin >> email;
                cout << "Unesite adresu: ";
                cin >> adresa;
                stablo->insertKey(new Record(id, ime, prezime, email, adresa));
                cout << "Uspesno dodat red!" << endl;
                break;
            case (3) :
                cout << "Unesite ID: ";
                cin >> id;
                int steps;
                stablo->findRecordById(id, steps, true);
                break;
            case (4) :
                cout << "Unesite ime: ";
                cin >> ime;
                stablo->findRecordsByName(ime);
                break;
            case (5) :
                cout << "Unesite ID: ";
                cin >> id;
                if (stablo->deleteById(id))
                   cout << "Uspesno obrisan red!" << endl;
                else
                    cout << "Red sa tim ID-om ne postoji!" << endl;
                break;
            case (6) :
                cout << "Unesite broj ID-eva: ";
                cin >> num_ids;
                ids.clear();
                for (int i = 0; i < num_ids; i++)
                {
                    cout << "Unesite ID: ";
                    cin >> id;
                    ids.push_back(id);
                }
                stablo->findRecordsByIds(ids);
                break;
            case (7) :
                stablo->print();
                break;
            case (8) :
                delete stablo;
                stablo = NULL;
                break;
            case (9) :
                cout << "Izasli ste iz programa" << endl;
                return 0;
        }
        cout << endl << endl << endl;
    }



    return 0;
}