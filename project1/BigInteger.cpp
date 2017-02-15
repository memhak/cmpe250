/*
Student Name: Mehmet Hakan Kurtoglu	
Project Number: 1
Operating System: Xubuntu
Compile Status: Yes
Program Status: Done
Notes: BigInteger class is implemented and project has been commited and pushed. Addition and multiplication
 work correctly and result gets printed into an output file.

*/

#include "BigInteger.h"
#include <algorithm>

    // constructs a new BigInteger object with specified number, passed as an integer value
    BigInteger::BigInteger(int number){
        int n = number;
        num = new LinkedList();
        while(n>=10){
            this->num->pushHead(n%10);
            n=n/10;
        }
        this->num->pushHead(n);
      }

    // constructs a new BigInteger object with specified number, which is passed as a string
    BigInteger::BigInteger(const string& bigInteger){
        char c;
        int length = bigInteger.length();
        int a;
        num = new LinkedList();
        for(int i=0;i<length;i++){
            a = bigInteger[i] - '0';
            this->num->pushHead(a);
        }
   }

    // adds two BigInteger arithmetically, returns result
	BigInteger BigInteger::operator+(const BigInteger &list){
		int carry = 0;
		int sum = 0;
		Node *pt1 = this->num->head;
		Node *pt2 = list.num->head;
        BigInteger ret;
        ret.num = new LinkedList();
		while(pt1 != NULL && pt2 != NULL){
            sum = pt1->data +pt2->data +carry;
            ret.num->pushTail(sum%10);
            carry = sum/10;
            pt1 = pt1->next;
            pt2 = pt2->next;
        }
        Node *bigger = NULL;
        if(pt1==NULL && pt2!=NULL)
            bigger = pt2;
        if(pt2==NULL && pt1!=NULL)
            bigger = pt1;
        while(bigger!=NULL){
            sum = bigger->data + carry;
            ret.num->pushTail(sum%10);
            carry = sum/10;
            bigger = bigger->next;
        }
        if(carry>0)
            ret.num->pushTail(carry);
        return ret;

	}

    // multiplies two BigIntegers and returns result
	BigInteger BigInteger::operator*(const BigInteger &list){
		BigInteger ret;
        ret.num = new LinkedList;
        BigInteger b(0);
        BigInteger b1(*this);
        Node* p = list.num->head;
        int c=0;
        while(p!=NULL){
            b=b1*p->data;
            for(int i=0;i<c;i++)
                b.num->pushHead(0);
            ret = ret+b;
            p=p->next;
            c++;
        }
        return ret;

	}

    // multiplies BigInteger with another integer value, returns the result
	BigInteger BigInteger::operator*(int i){
        LinkedList* l = new LinkedList;
        *l=*num;
        BigInteger* ret = new BigInteger;
        ret->num = l;
        Node* pt = ret->num->head;
        Node* pt1 = ret->num->head;
        int c=0;
        while(pt != NULL){
            pt->data = pt->data*i;
            pt = pt->next;
        }
        while(pt1 != NULL){
            pt1->data = pt1->data + c;
            c=pt1->data/10;
            pt1->data = pt1->data%10;
            pt1 = pt1->next;
        }
        if(c!=0){
            while(c>9){
                ret->num->pushTail(c%10);
                c=c/10;
            }
            ret->num->pushTail(c);
        }
        return *ret;
	}

    //Copy Constructor
    BigInteger::BigInteger(const BigInteger &other){
        delete this->num;
        this->num = new LinkedList();
		Node *temp = other.num->head;
		while (temp != NULL) {
        	this->num->pushTail(temp->data);
        	temp = temp->next;
    	}
	}

    // Assignment overload
	BigInteger& BigInteger::operator=(const BigInteger &list){
        delete this->num;
        num = new LinkedList();
        Node *temp = list.num->head;
        while (temp != NULL) {
            this->num->pushTail(temp->data);
            temp = temp->next;
        }
        return *this;
	}

    // destructor
	BigInteger::~BigInteger() { delete num; }

/*	ostream &operator<<(ostream &out, const BigInteger &bigInteger)  {
    	string str = "";
    	Node *head = bigInteger.num->head;
    	while (head) {
        	str += to_string(head->data);
        	head = head->next;
    	}
    	reverse(str.begin(), str.end());
    	if (str == "")
        	str = "0";
    	out << str;
    	return out;
	}
*/
