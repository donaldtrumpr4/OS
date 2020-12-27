#include "func.h"
using namespace std;



int main() {
    char* key = new char();
    int x = -1;
    cout << "DiskInfo v0.01" <<endl;

    while (1)
    {

        cout << "Menu:" <<endl;
        cout << "1. Get Drives list" <<endl;
        cout<< "2. Get Disk info - type disk path"<<endl;
        cout<<"3. Make new directory - type full path"<< endl;
        cout<<"4. Remove directory - type full path"<<endl;
        cout<<"5. Copy file - type full paths"<<endl;
        cout<<"6. Move file - type full paths"<<endl;
        cout<<"7. Create file - type full path"<<endl;
        cout<<"8. Get file attributes - type full path"<<endl;
        cout<<"9. Set file attributes - type full path"<<endl;
        cout<<"10. Get handle information - type full path"<<endl;
        cout<<"11. Get time information - type full path"<<endl;



        cin>>x;
        if (x==0) break;
        switch(x)
        {
            default:
            {
                GetDriveList();
                break;
            }
            case 1:
            {

                GetDriveList();
                break;
            }
            case 2:
            {
                GetDriveList();
                cin>>key;
                GetDiskInfo(key);
                break;
            }
            case 3:
            {
                cin>>key;
                CreateDir(key);
                break;
            }
            case 4:
            {
                cin>>key;
                RemoveDir(key);
                break;
            }
            case 5:
            {
                char* key2 = new char();
                cout<<"Enter existing file path"<<endl;
                cin>>key;
                cout<<"Enter new file path"<<endl;
                cin>>key2;
                Copy(key,key2);
                delete(key2);
                break;
            }
            case 6:
            {
                char* key2 = new char();
                cout<<"Enter existing file path"<<endl;
                cin>>key;
                cout<<"Enter new file path"<<endl;
                cin>>key2;
                Move(key,key2);
                delete(key2);
                break;
            }
            case 7:
            {
                cout<<"Enter file path"<<endl;
                cin>>key;
                Create(key);
                break;
            }
            case 8:
            {
                cout<<"Enter file path"<<endl;
                cin>>key;
                GetAtr(key);
                break;
            }
            case 9:
            {
                DWORD A;
                cout<<"Enter file path"<<endl;
                cin>>key;
                cout<<"Enter attributes"<<endl;
                cin>>A;
                SetAtr(key,A);
                break;
            }
            case 10:
            {
                cout<<"Enter file path"<<endl;
                cin>>key;
                GetByHandle(key);
                break;
            }
            case 11:
            {
                cout<<"Enter file path"<<endl;
                cin>>key;
                GetTime(key);
                break;
            }
        }
    }

    delete(key);


    return 0;
}