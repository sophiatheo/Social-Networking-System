#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <time.h>
#include <list>
#include <sstream>
#include <vector>

using namespace std;

class Wall;
class User;
class FriendRequest;

//���������� �� �������� ��� ��������� ���� �����
class Message {
    private:
        time_t timestamp;       //� ��� ��� ���������
        string message;         //�� ����������� ��� ���������
        User *pUser;            //� ������� ��� ����� ��� ��������
        map<User *, int>   usersLiked;   //������������ map ��� �� ������ ������� �� ������� ������� ����� Like ��� ������

    public:
        Message( string msg, User* pu) {
            time( &timestamp);
            message = msg;
            pUser = pu;
        }
        //���������� �� ����������� ��� string ���� �� ����������� ���� ����� �� ���������� �����.
        virtual string toString();

        //����� Like ��� ������.
        void Like( User *pUser);

        //���������� ��� ������ ��� Likes.
        int getNumberOfLikes()      { return usersLiked.size();}
};

//���������� ��� ���������� �� ���������� ��� ������.
class ReplyMessage : public Message {
    private:
        Message *pSrcMessage;           //�� ������ ���� ��� ����� ������� � ��������
    public:
        ReplyMessage( Message *pSourceMessage, string msg, User *pu) : Message( msg, pu) {pSrcMessage = pSourceMessage;}
        virtual string toString()  {
            return Message::toString() + " Reply from [" + pSrcMessage->toString() + "]";
        }
};

//���������� �� Wall ��� ������.
class Wall {
    private:
        list<Message *>     msgs;       //����� �� �� ��������
    public:
        void postToWall( Message *pMessage) {
            msgs.push_back( pMessage);
        }
        string toString() {
            list<Message *>::iterator  it = msgs.begin();
            if( it == msgs.end())
                return "";

            //��������� �� ����� string ��� �� �������� ��� ������.
            string ret( "WALL: ");
            while( it != msgs.end()) {
                Message  *pMessage = *it;
                ret += pMessage->toString();

                it++;

                //�� ��� ����� �� ��������� ���� ��� ������ ����� ��� ������������ ���������
                if( it != msgs.end())
                    ret += "|";

            }
            return ret;
        }

    list<Message *>  *getMessagesList() { return &msgs;}
};

//� ����� ��� ���������� �� ������
class  User {
    private:
        string  name;        //�� ����� ��� ������
        string  email;       //�� email ��� ������
        Wall    myWall;
public:
        User( string na, string em) {
            name = na;
            email = em;
        }

        string getEmail()       { return email;}
        string getName()        { return name;}
        Wall *getWall()         { return &myWall;}

        //������ ������ ������ ��� �� ������ pUser
        void sendFriendRequest( User *pUser);

        //������� �� ������ ������ pRequest.
        void acceptFriendRequest( FriendRequest *pRequest);

        //�������� �� ������ ������ pRequest.
        void rejectFriendRequest( FriendRequest *pRequest);

        //��������� ��� ���� ��� �� ����� ��� �����.
        void removeFriend( User *pUser);

        //������� ���� ����� �� ������ ��� �� ������ pSourceUser ��� ������ �� ����� ���� � ������ ��� ���� ������� ����� ���.
        Message *postToWall( string message, User *pSourceUser);

        //������� ���� ����� �� ������ ��� ��� ����.
        void postToWall( string message)                   {postToWall( message, this);}

        //������ ��� �������� ��� message ������ ��� ��������� pSourceMessage ��������� ���� ����� ��� ��� �� ������ pSourceUser.
        ReplyMessage *replyToMessage( Message *pSourceMessage, string message, User *pSourceUser);

        string toString();
};


//���������� ����������� ��� �� ������ ������.
class FriendRequest {
    private:
        User    *pUser;   //� ������ �������
        time_t  timeRequest;
public:
    FriendRequest( User *pu) {
        pUser = pu;
        time( &timeRequest);
    }

    //����������  �� ������ ��� ������� �� ������ ������.
    User    *getUser()     { return pUser;}

    //���������� �� ����������� ��������� ������ �� ����� string ���� �� �������� ���� �����.
    string toString() {
        string ret( pUser->getEmail());
        ret += " ";
        ret += ctime( &timeRequest);

        return ret;
    }
};

class UserExistsException : public exception {
    string email;
public:
    UserExistsException( string e) { email = e;}
    virtual const char * what () {
        return ("UserExists exception " + email).c_str();
    }
    ~UserExistsException() throw()  {
    }
};

class UserNotExistsException : public exception {
    string email;
public:
    UserNotExistsException( string e) { email = e;}
    virtual const char * what () {
        return ("UserNotExists exception " + email).c_str();
    }
    ~UserNotExistsException() throw()  {}
};

class SameUserException : public exception {
    User *pUser;
public:
    SameUserException( User *pu) { pUser = pu;}
    virtual const char * what () {
        return ("SameUserException " + pUser->getEmail()).c_str();
    }
};

class AlreadyLikeException : public exception{
    User *pUser;
    Message *pMessage;
public:
    AlreadyLikeException( User *pu, Message *pm) { pUser = pu; pMessage=pm;}
    virtual const char * what () const throw() {
        return ("AlreadyLikeException " + pUser->getEmail() + pMessage->toString()).c_str();
    }
};

class AlreadyFriendsException  : public exception {
    User *pUser1;
    User *pUser2;
public:
    AlreadyFriendsException( User *pu1, User *pu2) {
        pUser1 = pu1;
        pUser2 = pu2;
    }
    virtual const char * what () {
        return ("AlreadyFriendsException " + pUser1->getEmail() + " " + pUser2->getEmail()).c_str();
    }
};

class NoFriendsException  : public exception {
    User *pUser1;
    User *pUser2;
public:
    NoFriendsException( User *pu1, User *pu2) {
        pUser1 = pu1;
        pUser2 = pu2;
    }
    virtual const char * what () {
        return ("NoFriendsException " + pUser1->getEmail() + " " + pUser2->getEmail()).c_str();
    }
};


//����� ��� ���������� �� ������
class Network {
public:
    typedef list <User *>   aUsers;
    typedef vector <FriendRequest *>   aRequests;
private:
    list <User *>   users;                  //������� ��� �������� ���� �������
    map<string, User *> mapUsers;           //��� ������� ��������� ���� user �� ���� �� email ���
    map<User *, aUsers *> mapFriends;       //map ��� �� ������ ��� ����� ��� ����� ���
    map<User *, aRequests *> mapfriendRequests;    //�� �������� ������ ���� ��� ������.
    static Network *instance;               //singleton instance

public:
    //���� ����� ���� ��� �������� ���������� ��� ����������� ��� ������ Network ���  ���������� pointer �� ����. ���� �������� ������� ���������� ��� ���� pointer.
    static Network *getInstance() {
        if( instance != NULL)
            return instance;

        instance = new Network;

        cout << "NEW_INSTANCE------------------" << endl;
        return instance;
    }

    //���������� �� ����� �� �� �������� ������ ���� ��� ������ pUser.
    list<User *>    *getUsersList()     { return &users;}

    //���������� �� ����� �� �� �������� ������ ���� �� ������  pUser.
    vector<FriendRequest *>  *getFriendRequestsList( User *pUser) {
        map< User *, aRequests *>::iterator it = mapfriendRequests.find( pUser);
        if( it == mapfriendRequests.end())
            return NULL;
        else
            return it->second;
    }

    //��������� ��� �� �������� ��� �� ������
    void clear() {
        users.clear();
        mapUsers.clear();
    }

    //��������� ��� ��������� ������ ��� ������ ���������� ��� ����������� �� ����� ��� �� email ���..
    User *insertUser( string name, string email) {
        if( userExists( email))
            throw UserExistsException( email);

        User *u = new User( name, email);
        users.push_back( u);

        mapUsers.insert( make_pair( email, u));

        return u;
    }

    //���������� true �� �� email ����������� �� ������ ��� �������.
    bool userExists( string email) {
        map<string, User *>::iterator   it = mapUsers.find( email);

        if( it == mapUsers.end())
            return false;
        else
            return true;

        return false;
    }

    //��������� ��� ������
    void deleteUser( User *pUser) {
        deleteUser( pUser->getEmail());
    }

    //��������� ��� ������ �������� ��� pointer �� ���� �� ������.
    void deleteUser( string email) {
        //����� �� ����� ��� ����� ���.
        list<User *>::iterator v=users.begin();
        while( v != users.end()) {
            if( (*v)->getEmail() == email) {
                User    *pUser = *v;
                //������ �� �������� ��� ���� ������ ���
                list<User *>    *friends = getFriendsList( pUser);
                if( friends != NULL) {
                    list<User *>::iterator v2=friends->begin();
                    while( v2 != friends->end()) {
                        if( *v2 == pUser) {
                            friends->erase( v2);
                            break;
                        }
                    }
                }
                //�������� ��� ������������ ��� email �� �� ������
                mapUsers.erase( email);
                //������� �� ������ ��� �� ����� ��� �������
                users.erase( v);

                delete pUser;
                return;
            }
            v++;
        }

        //�� �� email ��� ����������� �� ������ ������� ��� ��������  UserNotExistsException.
        throw UserNotExistsException( email);
    }

    //���������� �� ����� ��� ����� ���� ������
    list<User *> *getFriendsList( User *pUser) {
        map<User *, aUsers *>::iterator  listIterator = mapFriends.find( pUser);

        if( listIterator == mapFriends.end())
            return NULL;
        else
            return listIterator->second;
    }

    //���������� true � false ������� �� �� �� ��� ������� ����� �����.
    bool areFriends( User *pUser1, User *pUser2) {
        if( pUser1 == NULL || pUser2 == NULL)
            return false;

        //������� �� �� �� � ������ ������� ���� ���� �� ������� ������
        map<User *, aUsers *>::iterator a = mapFriends.find( pUser1);
        if( a == mapFriends.end())
            return false;

        list<User *> *pFriends = a->second;
        list<User *>::iterator v = pFriends->begin();
        while( v != pFriends->end()) {
            if( *v == pUser2) {
                return true;
            }
            v++;
        }

        return false;
    }

    //����� ������ ��� �������
    //�� �� ���  ������� ������������ ���� ���� ������ ������� ��� �������� SameUserException.
    //�� ����� ��� ����� ������� ��� �������� AlreadyFriendsException.
    void makeFriends( User *pUser1, User *pUser2) {
        if( pUser1->getEmail() == pUser2->getEmail())
            throw SameUserException( pUser1);

        if( areFriends( pUser1, pUser2))
            throw AlreadyFriendsException( pUser1, pUser2);

        //�������� �� ������� ������ ����� ������ ��� ������.
        map<User *, aUsers *>::iterator listIterator = mapFriends.find( pUser1);
        aUsers  *myFriends;
        if( listIterator == mapFriends.end()) {
            myFriends = new aUsers;
            mapFriends.insert( make_pair(pUser1,myFriends));
        } else
            myFriends = listIterator->second;

        myFriends->push_back( pUser2);

        //�������� ��� ����� ������ ����� ������ ��� ��������.
        listIterator = mapFriends.find( pUser2);
        if( listIterator == mapFriends.end()) {
            myFriends = new aUsers;
            mapFriends.insert( make_pair(pUser2,myFriends));
        } else
            myFriends = listIterator->second;

        myFriends->push_back( pUser1);
    }

    //������� ��� ������ ���� ������� pUser1 ��� pUser2.
    //�� ��� ����� ����� ������� ��� �������� NoFriendsException.
    void removeFriend( User *pUser1, User *pUser2) {

        if( !areFriends( pUser1, pUser2))
            throw NoFriendsException( pUser1, pUser2);

        //�������� �� ������� ������ ����� ������ ��� ������.
        map<User *, aUsers *>::iterator mylist = mapFriends.find( pUser1);

        //������� �� ������� ������ ��� �� ����� ��� ����� ��� ������ ������
        list<User *>::iterator v=mylist->second->begin();
        while( v != mylist->second->end()) {
            if( *v == pUser2) {
                mylist->second->erase( v);
                break;
            }
            v++;
        }
    }

    //�������� �� email ���������� �� ������ ���� ����� ����������� ���� �� email.
    //�� �� email ��� ����������� �� ������ ���������� NULL.
    User *getUser( string email) {
        list<User *>::iterator v=users.begin();
        while( v != users.end()) {
            if( (*v)->getEmail() == email) {
                return *v;
            }
            v++;
        }

        return NULL;
    }

    //���������� true � false ������� �� �� �� ��� email ������������ �� ������� ��� ����� �����.
    bool areFriends( string user1, string user2) {
        return areFriends( getUser( user1), getUser( user2));
    }

    //���������� �� ����� string �� ����������� ��� �������.
    string  toString() {
        string ret;

        list<User *>::iterator v=users.begin();
        while( v != users.end()) {
            ret += (*v)->toString();
            ret += "\n";
            v++;
        }

        return ret;
    }

    //������� ������ ������ � �������� pUser1 ��� ������ pUser2
    //�� ����� ��� ����� ������� ��� �������� AlreadyFriendsException.
    void sendFriendRequest( User *pUser1, User *pUser2) {
        if( areFriends( pUser1, pUser2))
            throw AlreadyFriendsException( pUser1, pUser2);

        //������� �� �� �� � pUser2 ���� ��� ����� �� ������� ������
        map<User *,aRequests *>::iterator it = mapfriendRequests.find( pUser2);
        aRequests  *a;
        if( it == mapfriendRequests.end()) {
            //��� ���� ����� �� ������� �������� ����� ��� ������� ��� ���
            a = new aRequests;
            mapfriendRequests.insert( make_pair( pUser2, a));
        }else
            a = it->second;     //���� ����� �� ������� ��������

        //�������� �� ������ ��� ����� �� �� ��������.
        FriendRequest *pRequest = new FriendRequest( pUser1);
        a->push_back( pRequest);
    }

    void aceptRejectFriendRequest( User *pUser, FriendRequest *pRequest, bool doAccept) {
cout << "aceptRejectFriendRequest" << endl;
        if( doAccept)
            makeFriends( pUser, pRequest->getUser());

        vector<FriendRequest *> *pRequests = getFriendRequestsList( pUser);
        vector<FriendRequest *>::iterator it = pRequests->begin();
        while( it != pRequests->end()) {
            cout << "request:" << pRequest->getUser()->getEmail() << endl;
            if( *it == pRequest) {
                pRequests->erase( it);
                cout << "Request was removed" << endl;
                //���� �� ����� ��� ���������� �� �������� �� ������.
                return;
            }
            it++;
        }
    }

    void acceptFriendRequest( User *pUser, FriendRequest *pRequest) {aceptRejectFriendRequest(pUser, pRequest, true);}
    void rejectFriendRequest( User *pUser, FriendRequest *pRequest) {aceptRejectFriendRequest(pUser, pRequest, false);}

    bool hasFriendRequest( User *pUser1, User *pUser2) {
        vector<FriendRequest *> *pRequests = getFriendRequestsList( pUser1);
        if( pRequests == NULL)
            return false;

        for(vector<FriendRequest *>::iterator it = pRequests->begin(); it != pRequests->end(); it++) {
            if( (*it)->getUser() == pUser2)
                return true;

        }

        return false;
    }

    //���������� ����� �� ���� ������� �������.��� pUser1 ��� pUser2.
    list<User *>  *getCommonFriends( User *pUser1, User *pUser2) {
        list<User *>    *pCommon = new list<User *>;
        list<User *>    *friends = getFriendsList( pUser1);
        list<User *>::iterator   it = friends->begin();
        while( it != friends->end()) {
            if( !areFriends( *it, pUser2))
                continue;
            pCommon->push_back( *it);

            it++;
        }

        return pCommon;
    }
};

Network *Network::instance=NULL;


//���������� �� ����� ��� ������������.
class Menu {
public:
    //��������� �� �������� ����� ��� ���� ����� ������ ��������� �� email  ��� ������ ��� ���� ����� �login".
    int showMainMenu( User *pUser) {
        //�������� �� �����.
        cout << endl << endl << "    MENU    user:" << pUser->getEmail() << endl;
        cout << "1. See your wall" << endl;
        cout << "2. See friends wall" << endl;
        cout << "3. Send friend request" << endl;
        cout << "4. Accept/Reject friend request" << endl;
        cout << "5. See my friends" << endl;
        cout << "6. Back" << endl;
        cout << "7. Login as another user" << endl;
        cout << "8. Exit" << endl;
        cout << "Select (1-8): ";

        //�������� ��� �������� ��� �� ������.
        int epilogi;
        cin >> epilogi;

        return epilogi;
    }

    //������ ��� ������������� ��� email ��� ���������� �� ������ ���� ����� ����������� �� email ����.
    //�� �� email ��� �������������� ��� ����������� �� ������ ���������� �� email.
    User *inputUsername() {
        string email;

        User *pUser=NULL;

        //��������� �� ������ ����� �� ����� email ��� �� ������ �� ������� ������.
        while( pUser == NULL) {
            cout << "Give username:";
            getline( cin, email);
            if( email.size() == 0)
                getline( cin, email);

            pUser = Network::getInstance()->getUser( email);
            if( pUser == NULL)
                cout << "No user with email #" << email << "#" << endl;
        }

        return pUser;
    }

    //��������� ���� ����� ���� ������ ��� ������ pUser ��� ������ ��� ������� ���� ��� ������.
    User *selectFriend( User *pUser) {
        cout << "SELECT FRIEND" << endl;
        list<User *>    *friends = Network::getInstance()->getFriendsList( pUser);
        if( friends == NULL) {
            cout << "No friends" << endl;
            return NULL;        //��� ���� ������
        }
        if( friends->size() == 0) {
            cout << "No friends" << endl;
            return NULL;        //��� ���� ������.
            }

        //������ �� ����� �� ����� ���� ������ ��� pUser.
        list<User *>::iterator  it = friends->begin();
        int count = 0;
        while( it != friends->end()) {
            count = count + 1;
            cout << count << ". " + (*it)->getEmail() << endl;
            it++;
        }
        cout << "0. Back" << endl;

        int epilogi = 0;
        while( (epilogi <= 0) || (epilogi > count)) {
            cout << "Select 0-" << count << "): ";
            cin >> epilogi;
            if( epilogi == 0)
                return NULL;
        }

        it = friends->begin();
        count = 0;
        while( it != friends->end()) {
            count = count + 1;
            if( count == epilogi)
                return *it;
            it++;
        }

        return NULL;
    }

    //��������� �� ����� �� ��� �������� ��� ������� ��� ����� (post, reply, like).
    int showWallMenu( User *pUser) {
        cout << endl;
        cout << "a. Post a message" << endl;
        cout << "b. Reply to a message" << endl;
        cout << "c. Like" << endl;
        cout << "0. Back" << endl;

        string selection;
        cout << "Select (a,b,c,0): ";
        for(;;) {
            getline( cin, selection);
            if( selection == "a")
                return 1;
            if( selection == "b")
                return 2;
            if( selection == "c")
                return 3;
            if( selection == "0")
                return 0;
        }
    }

    //������ ��� ������ ���� ����� ��� ������ pUser2 ��� �� ������ pUser1.
    void DoPostMessage( User *pUser1, User *pUser2) {
        string s;

        cout << endl << "POSTING A MESSAGE TO WALL" << endl;
        cout << "Give a message:";

        getline( cin, s);
        pUser2->postToWall( s, pUser1);
    }

    //��������� �� ����� ��� ��������� ��� pUser2 ��� ������ ��� ������� ���� ��������� ���� � ������ pUser1 �� ����� Like �� ����.
    //���������� true �� �� ������ �� ���� ��� ���� ����
    bool DoLike( User *pUser1, User *pUser2) {
        string s;

        cout << endl << "LIKE A MESSAGE OF FRIEND'S WALL" << endl;
        list<Message *>  *msgs = pUser2->getWall()->getMessagesList();

         //������ ��� ��� �� �������� �������� ������� ��� ������� ������.
        list<Message *>::iterator it = msgs->begin();
        int count = 0;
        while( it != msgs->end()) {
            count = count + 1;
            cout << count << ". " << (*it)->toString() << endl;
            it++;
        }
        if( count == 0) {
            cout << "No messages for like" << endl;
            return false;
        }
        cout << "0. No Like" << endl;
        int epilogi=-1;
        while( epilogi < 0 || epilogi > count) {
            cout << "Select: ";
            cin >> epilogi;
        }
        if( epilogi == 0)
            return true;     //������� �� ��� ����� ������ Like.
        count = 0;
        it = msgs->begin();
        while( it != msgs->end()) {
            count = count + 1;
            if( count == epilogi) {
                cout << "DoLike " << (*it)->toString() << " USER: " << pUser1->getEmail() << endl;
                (*it)->Like( pUser1);
                break;
            }
            it++;
        }

        //������ ��� ����� ��� ������ pUser2 ��� �� ����� ��� ����� Like.
        cout << "WALL of user " << pUser2->getEmail() << endl;
        cout << pUser2->getWall()->toString() << endl;

        return false;
    };

    //��������� �� ����� ��� ��������� ��� pUser2 ��� ������ ��� ������� ���� ��� ����.
    //��� �������� ������ �� ������� ���� �� �� �������� ��� �������� � pUser1 ���� ����� ��� pUser2.
    void DoReply( User *pUser1, User *pUser2) {
        cout << endl << "REPLY A MESSAGE OF FRIEND'S WALL" << endl;
        list<Message *>  *msgs = pUser2->getWall()->getMessagesList();

        //������ ��� ��� �� �������� �������� ������� ��� ������� ������.
        list<Message *>::iterator it = msgs->begin();
        int count = 0;
        while( it != msgs->end()) {
            count = count + 1;
            cout << count << ". " << (*it)->toString() << endl;
            it++;
        }
        if( count == 0) {
            cout << "No messages for reply" << endl;
            return;
        }
        cout << "0. No reply" << endl;
        int epilogi=-1;
        while( epilogi < 0 || epilogi > count) {
            cout << "Select: ";
            cin >> epilogi;
        }
        cout << "Epilogi=0" << epilogi << endl;
        if( epilogi == 0)
            return;     //������� �� ��� ����� ������ Like.
        count = 0;
        it = msgs->begin();
        while( it != msgs->end()) {
            count = count + 1;
            if( count == epilogi) {
                cout << "Reply " << (*it)->toString() << " USER: " << pUser1->getEmail() << endl;
                string msg;
                getline( cin, msg);
                cout << "Message for reply: ";
                getline( cin, msg);
                if( msg.size() != 0){
                    cout << "postToWall" << endl;
                    pUser2->replyToMessage( *it, msg, pUser1);
                    cout << "NEW WALL";
                    cout  << endl << pUser2->getWall()->toString() << endl;
                }
                break;
            }
            it++;
        }

        //������ ��� ����� ��� ������ pUser2 ��� �� ����� ��� ����� Like.
        cout << "WALL of user " << pUser2->getEmail() << endl;
        cout << pUser2->getWall()->toString() << endl;
    }

    //������� ���� ������ ��� pUser.
    void printFriends( User *pUser) {
        list<User *> *myFriends=Network::getInstance()->getFriendsList( pUser);
        if( myFriends != NULL) {
            cout << " FRIENDS of "<< pUser->getEmail() << ": " << endl;
            list<User *>::iterator v=myFriends->begin();
            while( v != myFriends->end()) {
                cout <<  " " + (*v)->getEmail() + " " + (*v)->getName() << endl;
                v++;
            }
            cout << endl;
        } else
            cout << "No friends" << endl;
    }

    //��������� ����� ��� ����� ����� �� ��� pUser � ��� ��� ����� ������� �������� ������ � ��� ���� ������� �������� ������ ��� ������ ��� ������� ���� ��� ������
    // ���� �� ��� ������ ������ ������.
    void sendFriendRequest( User *pUser) {
        Network *network = Network::getInstance();
        list<User *>  *pUsers = network->getUsersList();
        int count = 0;
        vector<User *>  users;
        cout << "SELECT A USER TO SEND FRIEND REQUEST" << endl;
        for( list<User *>::iterator it = pUsers->begin(); it != pUsers->end(); it++) {
            //������� �� ����� � ����� �������.
            if( *it == pUser)
                continue;

            //������� �� ����� ��� �����.
            if( network->areFriends( pUser, *it))
                continue;   //����� ����� ����� ��� ��� ������

            //������� �� ��� ���� ������� ������ ������
            if( network->hasFriendRequest( pUser, *it))
                continue;

            //������� �� ��� ��� ������� ������ ������
            if( network->hasFriendRequest( *it, pUser))
                continue;

            count++;
            cout <<  count << ". " + (*it)->getEmail() + " " + (*it)->getName() << endl;
            users.push_back( *it);
        }
        if( count == 0) {
            cout << "No users to send friend requests." << endl;
            return;
        }
        int epilogi=-1;
        while( epilogi < 0 || epilogi > count) {
            cout << "Select (0:None 1-" << count << "):";
            cin >> epilogi;
        }
        if( epilogi == 0)
            return; //��� ����� �� ������� ������ ������.
        User *pNewUser = users.at( epilogi - 1);
        pUser->sendFriendRequest( pNewUser);
    }

    //��������� ���� ����� �� �������� ������ ���� ��� pUser ��� ������ ������ ��� ������� ���� ��� ����.
    //��� �������� ������ �� �� ����� ������� � �������� ��� ��������� ������.
    void AcceptRejectFriendRequest( User *pUser) {
        Network *network = Network::getInstance();

        vector<FriendRequest *>  *pRequests = network->getFriendRequestsList( pUser);

        //������� ��� �� �� �������� ������� �������� ������
        if( pRequests == NULL) {
            cout << "No friend requests" << endl;
            return;
        }

        //������� ��� �� �� �������� ������� �������� ������
        vector<FriendRequest *>::iterator  it = pRequests->begin();
        if( it == pRequests->end()) {
            cout << "No friend requests" << endl;
            return;
        }

        //������� �� �������� ������ ��� ��� ����� �����.
        cout << "FRIEND REQUESTS: " << endl;
        vector<FriendRequest *>::iterator v=pRequests->begin();
        int count = 0;
        while( v != pRequests->end()) {
            count++;
            cout << count << ". " << (*v)->toString();
            v++;
        }
        //�������� ��� ������� ��� ������.
        int epilogi = -1;
        while ( (epilogi < 0) || (epilogi > count)) {
            cout << "Select (0:None 1-" << count << "): ";
            cin >> epilogi;
        }
        if( epilogi == 0)
            return; //��� ����� �� ������� ������ ������.
        User    *pUser2 = pRequests->at( epilogi - 1)->getUser();

        int epilogi2 = -1;
        while( epilogi2 < 0 || epilogi2 > 2) {
            cout << endl << "1. Accept" << endl;
            cout << "2. Reject" << endl;
            cout << "0. Ignore" << endl;
            cout << "Select (1,2,0): ";
            cin >> epilogi2;
        }
        if( epilogi2 == 0)
            return;
        if( epilogi2 == 1) {
            pUser->acceptFriendRequest( pRequests->at( epilogi-1));
            //�������� ��� �����
            printFriends( pUser);
        }else if( epilogi2 == 2) {
            cout << "GOTO REMOVE" << endl;
            pUser->rejectFriendRequest( pRequests->at( epilogi-1));
        }
    }
};

//������ ��� ���� ��� ������
void User::removeFriend( User *pUser) {
    Network::getInstance()->removeFriend( this, pUser);
}

//� ������ ������� ������� ������ ������ ���� pUser
void User::sendFriendRequest( User *pUser) {
    Network::getInstance()->sendFriendRequest( this, pUser);
}

//���������� �� ����������� ��� ������ �� ����� string ���� �� �������� ���� �����.
string User::toString() {
    string ret = name + " " + email;

    list<User *> *myFriends=Network::getInstance()->getFriendsList( this);
    if( myFriends != NULL) {
        ret += " FRIENDS: ";
        list<User *>::iterator v=myFriends->begin();
        while( v != myFriends->end()) {
            ret += " " + (*v)->getEmail();
            v++;
        }
    }

    //������ �� ����� ��� ��������� ������.
    vector<FriendRequest *> *pRequests = Network::getInstance()->getFriendRequestsList( this);
    if( pRequests != NULL) {
        vector<FriendRequest *>::iterator it = pRequests->begin();
        if( it != pRequests->end()) {
            ret += " FRIEND_REQUESTS[ ";
            while( it != pRequests->end()) {
                ret += (*it)->getUser()->getEmail();
                ret += " ";
                it++;
            }
            ret += "]";
        }

    }

    return ret + " " + myWall.toString();
}

//� ������� pSourceUser ���������� �� ������ message ���� ����� ��� ��������� ������.
//�� � pSourceUser ��� ����� ����� ������� �������� NoFriendsException.
Message *User::postToWall( string message, User *pSourceUser) {
    if( pSourceUser != this) {
        if( !Network::getInstance()->areFriends( this, pSourceUser))
            throw NoFriendsException( this, pSourceUser);
    }
    Message *pMessage = new Message( message, pSourceUser);
    myWall.postToWall( pMessage);

    return pMessage;
}

void User::acceptFriendRequest( FriendRequest *pRequest) {
    Network::getInstance()->acceptFriendRequest( this, pRequest);
}

void User::rejectFriendRequest( FriendRequest *pRequest) {
    Network::getInstance()->rejectFriendRequest( this, pRequest);
}

ReplyMessage *User::replyToMessage( Message *pSourceMessage, string msg, User *pSourceUser) {
    cout << "REPLYTOMESSAGE" << endl;

    ReplyMessage  *pMessage = new ReplyMessage( pSourceMessage, msg, pSourceUser);
    cout << "REPLY=" << pMessage->toString() << endl;
    myWall.postToWall( pMessage);
}


//���������� �� ����������� ��� string ���� �� ����������� ���� ����� �� ���������� �����.
string Message::toString() {
    string ret = message + " (" + pUser->getEmail() + ") Likes: ";
    if( usersLiked.size() == 0)
        ret += "0";
    else
    {
        ostringstream  s;
        s << usersLiked.size();
        ret += s.str();
    }

    return ret;
}

//� ������� pUser ����� Like ��� ������.
//�� ���� ��� ����� Like ������� �������� AlreadyLikeException.
void Message::Like( User *pUser) {
    map<User *, int>::iterator   it  = usersLiked.find( pUser);
    if( it != usersLiked.end())
        throw AlreadyLikeException( pUser, this);
    usersLiked.insert( make_pair( pUser, 1));

    cout << endl << "Number of likes: " << getNumberOfLikes() << endl;
}

//��������� ��� ������ ����������� ��������.
void dotest() {
    try
    {
        Network *network = Network::getInstance();
        network->clear();
        cout << "Insert User u1@a.gr";User *pUser1=network->insertUser( "u1", "u1@a.gr"); cout << " ok" << endl;
        cout << "Insert User u2@a.gr";User *pUser2=network->insertUser( "u2", "u2@a.gr"); cout << " ok" << endl;
        cout << "Insert User u3@a.gr";User *pUser3=network->insertUser( "u3", "u3@a.gr"); cout << " ok" << endl;
        cout << "Insert User u4@a.gr";User *pUser4=network->insertUser( "u4", "u4@a.gr"); cout << " ok" << endl;
        cout << "Insert User u5@a.gr";User *pUser5=network->insertUser( "u5", "u5@a.gr"); cout << " ok" << endl;
        cout << "Insert User u6@a.gr";User *pUser6=network->insertUser( "u6", "u6@a.gr"); cout << " ok" << endl;
        cout << "Insert User u7@a.gr";User *pUser7=network->insertUser( "u7", "u7@a.gr"); cout << " ok" << endl;
        cout << "Insert User u8@a.gr";User *pUser8=network->insertUser( "u7", "u8@a.gr"); cout << " ok" << endl;
        cout << "Insert User u9@a.gr";User *pUser9=network->insertUser( "u7", "u9@a.gr"); cout << " ok" << endl;
        cout << "MakeFriends u1@a.gr u3@a.gr";network->makeFriends( pUser1, pUser3);cout << " ok" << endl;
        cout << "MakeFriends u1@a.gr u4@a.gr";network->makeFriends( pUser1, pUser4);cout << " ok" << endl;
        cout << "Delete User u2@a.gr";network->deleteUser( "u2@a.gr"); cout << " ok" << endl;
        cout << "Delete User u5@a.gr";network->deleteUser( pUser5); cout << " ok" << endl;
        cout << "AreFriends u1@a.gr u2@a.gr (no) ret=" << network->areFriends( pUser1, pUser2);cout << " ok" << endl;
        cout << "AreFriends u1@a.gr u4@a.gr (yes) ret=" << network->areFriends( pUser1, pUser4);cout << " ok" << endl;
        cout << "SendFriendRequest u3@a.gr u4@a.gr "; network->sendFriendRequest( pUser3, pUser4); cout << " ok" << endl;
        cout << "SendFriendRequest u3@a.gr u4@a.gr "; network->sendFriendRequest( pUser6, pUser1); cout << " ok" << endl;
        cout << "Post to Wall of u1@a.gr from u4@a.gr"; pUser1->postToWall( "Hello1", pUser4); cout << " ok" << endl;
        cout << "Post to Wall of u1@a.gr from u4@a.gr"; pUser1->postToWall( "Hello2", pUser4); cout << " ok" << endl;
        cout << "Post to Wall of u3@a.gr from u1@a.gr"; Message *pMessage = pUser3->postToWall( "Buy1", pUser1); cout << " ok" << endl;
        cout << "Post to Wall of u3@a.gr from u1@a.gr"; pUser3->postToWall( "Buy2", pUser1); cout << " ok" << endl;
        cout << "LiKE message Bye from u1@a.gr ";pMessage->Like( pUser1);

        list<User *>    *pCommon = network->getCommonFriends(pUser3, pUser4);
        cout << "COMMON FRIENDS " << pUser3->getEmail() << "-" << pUser4->getEmail() << ": ";
        for( list<User *>::iterator it = pCommon->begin(); it != pCommon->end(); it++) {
            cout << (*it)->getEmail() << " ";
        }
        cout << endl;


        cout << "USERS" << endl << network->toString() << "End of tests" << endl;
    }catch (exception& e)
    {
        cout << e.what() << endl;
    }
}

//��������� �� ����� ��� ������� ���� ������� ��� ������.
void domenu() {
    Menu    m;
    int     epilogi = -1;
    User    *pUser = Network::getInstance()->getUser( "u1@a.gr");

    bool callSelectFriend = true;
    while( true) {
        switch( epilogi) {
            case 1://See your wall;
                cout << endl << endl << endl << endl;
                cout << "WALL of user " << pUser->getEmail() << endl;
                cout << pUser->getWall()->toString() << endl;
                break;
            case 2://See friends wall;
                cout << endl << endl << endl << endl;
                User *pUser2;
                if( callSelectFriend) {
                    pUser2 = m.selectFriend( pUser);
                    if( pUser2 == NULL)
                        break;
                }
                callSelectFriend = true;
                if( pUser2 != NULL) {
                    cout << "WALL of user " << pUser2->getEmail() << endl;
                    cout << pUser2->getWall()->toString() << endl;
                    epilogi = m.showWallMenu( pUser2);
                    switch( epilogi) {
                        case 1: //Post a message
                            m.DoPostMessage( pUser, pUser2);
                            break;
                        case 2: //Reply to a message
                            m.DoReply( pUser, pUser2);
                            break;
                        case 3: //Like
                            if( m.DoLike( pUser, pUser2)) {
                                epilogi = 2;
                                callSelectFriend = false;
                                continue;
                            }
                            break;
                        case 0:
                            epilogi = 2;
                            continue;
                    }
                }
                break;
            case 3://Send friend request;
                m.sendFriendRequest( pUser);
                break;
            case 4://Accept/reject friend request
                m.AcceptRejectFriendRequest( pUser);
                break;
            case 5: //See my friends.
                m.printFriends( pUser);
                break;
            case 6:     //Back.
            case 7:     //Login as another user.
                pUser = m.inputUsername();
                break;
            case 8:
                cout << "EXIT" << endl;
                exit( 1);   //����� ������������
        }

        epilogi = m.showMainMenu( pUser);
    }
}

int main() {
    dotest();


    for(;;) {
        try {
            domenu();
        }catch (exception& e)
        {
            cout << e.what() << endl;
        }
    }

    return 0;
}
