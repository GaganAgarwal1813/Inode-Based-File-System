#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include<bits/stdc++.h>

# define No_of_Inode 78644

# define datablock 131072

# define block_size 4096

# define inodeandsuperblock 1664


using namespace std;

ofstream myfile;

FILE *diskpointer;

int  writevalcount=0;


void writelog(string s)
{
    myfile<<s<<endl;
    return;
}


struct super_block 
{
   
    bool lisfreeinode[78644];    // to check which inode no is free to assign to file true== Reserved and false== FREE
    bool lisfreedblock[131072]; //to check which data block is free to allocate to file  total no of blocks = 512M/4096 = 131072

};


struct inode 
{
    int fsize;
    int pointer[12]; 
    char file_name[30]; 
    int inode_num;     
};


struct super_block sb;



struct inode arrinode[78644];


vector<int> finodevect;

char* dname[100];

vector<int> freefilesdesvect; 


vector<int> fdatablockvect;  

map<int, string> inodefilemap;  

map<string, int> fileinodemap;  

char fname[50];

map<int, pair<int, int>> filedescripmap;

map<int, int> filedescripmodemap;  

int ofilecnt = 0;   

static int active = 0;



int DiskCreate(string diskname)
{
    if(access( &diskname[0], F_OK ) == -1)
    {
        writelog("Starting Disk Creation");

        int n = diskname.length();

        char dname[n+1];

        strcpy(dname, diskname.c_str());

        FILE* diskpointer=fopen(dname,"wb+");

        writelog("Disk Pointer Created");
        char buffer[4096];
        memset(buffer, 0, 4096);

        writelog("Buffer has been initalised to NULL successfully");



        for(int i=0;i<131072;i++)
        {
            fwrite(buffer, 1, 4096, diskpointer); 
        }

        struct super_block sb;
        writelog("Buffer used to Initialise Disk Null");

        

        for (int i = 0; i < inodeandsuperblock; i++)
            sb.lisfreedblock[i] = true;
        
        writelog("Inode Blocks reserved");

        for (int i = inodeandsuperblock; i < datablock; i++)
            sb.lisfreedblock[i] = false; 

        writelog("Blocks other than Inode Blocks are freed");

        for (int i = 0; i < No_of_Inode; i++)
            sb.lisfreeinode[i] = false;

        writelog("All Inodes made free Successfully");

        for (int i = 0; i < No_of_Inode; ++i)
        {
            writevalcount=0;
            for (int j = 0; j < 12; j++)
            {
                writevalcount=1;
                arrinode[i].pointer[j] = -1;
                writevalcount++;
            }
        }

        writelog("Inode array set as empty");


        int len = sizeof(struct super_block);
        char superblockbuff[len];

        memset(superblockbuff, 0, len);
        
        writelog("Super Block Buffer set to NULL");

        fseek(diskpointer, 0, SEEK_SET);
        
        writelog("Disk Pointer Moved to Starting");
        memcpy(superblockbuff, &sb, sizeof(sb));

        writelog("Data copied to Buffer");

        fwrite(&sb,sizeof(struct super_block), 1,diskpointer);

        writelog(" Super Block stored at the start ");  


        len = sizeof(arrinode);
        char directbuff[len];
        memset(directbuff, 0, len);


       
        memcpy(directbuff, arrinode, len);

        writelog("Data copied to Buffer (Inode Mapping)");

        
        fwrite(arrinode, sizeof(struct inode),No_of_Inode,diskpointer);

        fclose(diskpointer);

        writelog("Disk Pointer Closed ");


        cout <<"Virtual Disk Created!!!" << endl;

        writelog("Virtual Disk Created Successfully");



        return 1;


      
    }
    writelog("Disk already Created");
    cout<<"Disk already Created"<<endl;
    return -1;
}



int mountDisk(string diskname)
{

    int n = diskname.length();

    char dname[n+1];

    strcpy(dname, diskname.c_str());

    diskpointer=fopen(dname,"rb+");
    
    writelog("Disk Pointer Created in Mount");

    if (diskpointer != NULL)
    {
        writelog("Disk Pointer pointing to the Disk Present");
        

        char superblockbuff[sizeof(sb)];
        memset(superblockbuff, 0, sizeof(sb));

        writelog("Super Block Buffer set NULL ");

        

        fread(&sb,sizeof(struct super_block), 1,diskpointer);

       
        writelog("Data Read from the File copied to buffer Successfully");


        int len = sizeof(arrinode);

       
        char inodebuffarr[len];
        memset(inodebuffarr, 0, len);

        writelog("Empty Buffer for global structure is created");

        

        fread(arrinode, sizeof(struct inode), No_of_Inode ,diskpointer);
     

        writelog("Data Copied for global Structure in the Buffer");





        for (int i = No_of_Inode - 1; i >= 0; i--)
        {
            if (sb.lisfreeinode[i] == false)
            {
               
                finodevect.push_back(i);
            }
            else
            {
                fileinodemap[string(arrinode[i].file_name)] = arrinode[i].inode_num;

                writevalcount=2;
                inodefilemap[arrinode[i].inode_num] = string(arrinode[i].file_name);

                

                
            }
        }


        writelog("All fnames are stored into the map");
        for (int i =datablock - 1; i >= inodeandsuperblock ; i--)
        { 
            if (sb.lisfreedblock[i] == false)
            {
              
                fdatablockvect.push_back(i);
            }
        }

        writelog("File Data Vector denoting free data blocks is created");

        
        for (int i = 31; i >= 0; i--)
        {
            freefilesdesvect.push_back(i);
        }

        writelog("File Descriptor Vector denoting free files is created");

        cout << "Disk is mounted Successfully" << endl;
        writelog("Disk Mounting is Done");
        active = 1;
        return 1;



        
    }
    else
    {
        cout <<"Disk does not exist " << endl;
        
        return 0;
    }
}


void clearallds()
{
    finodevect.clear();
    fdatablockvect.clear();
    freefilesdesvect.clear();
    filedescripmodemap.clear();
    filedescripmap.clear();
    fileinodemap.clear();
    inodefilemap.clear();
}


void freeinodevect()
{
    writevalcount=0;
    for (unsigned int i = 0; i < finodevect.size(); i++)
    {
        sb.lisfreeinode[finodevect[i]] = false;
        writevalcount=3;
    }

    writevalcount++;
}


bool checkinodespace()
{
    return (finodevect.size() == 0);
}

bool checkdatablockespace()
{
    return (fdatablockvect.size() == 0);
}



int checkopen(int fdescipt)
{
    if (filedescripmap.find(fdescipt) == filedescripmap.end())
    {
        cout  << "File is not opened yet Please open it before Reading" <<  endl;
        return -1;
    }
    return 0;
}

int checkreadmodeopen(int fdescipt)
{
    if (filedescripmodemap[fdescipt] != 0)
    {
        cout  << "Read File Error : file with descriptor " << fdescipt << " is not opened in read mode !!!" << endl;
        return -1;
    }
    return 0;
}











int unmounthelperfun()
{
    
    fseek(diskpointer, 0, SEEK_SET);
    

    

    fwrite(&sb,sizeof(struct super_block), 1,diskpointer);

    
    
   

    fwrite(arrinode, sizeof(struct inode),No_of_Inode,diskpointer);

    

    
    return 0;




}



















int write_into_file(int fd, int mode)
{



    if (filedescripmap.find(fd) == filedescripmap.end())
    {
        cout << "Write File Error : File descriptor " << fd << " doesn't exist !!!" << endl;
        return -1;
    }

    if(mode==0)
    {
        cout<<"File Not opened in Correct Mode"<<endl;
        return -1;
    }


    

    cout << "Enter file content : " << endl;
    cout.flush();
    string s;

    string tmp_s;

    while(tmp_s!="FileClose")
    {
        getline(cin, tmp_s);
        if(tmp_s=="FileClose")
            break;
        s += (tmp_s + "\n");
    }

   

    long long int len=s.length();

    if(len>(block_size*12))
    {
       
        cout<<"Data will be truncated after 48 KB."<<endl;
    }

    int num_of_blocks=ceil((len*(1.0))/block_size);

    for(int i=0;i<num_of_blocks;i++)
    {
        if(i>=12)
            break;
        string data=s.substr(i*4096,4096);

       

        int cinodenumber=filedescripmap[fd].first;
        if(arrinode[cinodenumber].pointer[i]==-1)
        {
            int freedatablocknum=fdatablockvect.back();
            fdatablockvect.pop_back();

            arrinode[cinodenumber].pointer[i]=freedatablocknum;

            sb.lisfreedblock[freedatablocknum]=true; // to check

            fseek(diskpointer,arrinode[cinodenumber].pointer[i]*4096,SEEK_SET);

            myfile<<"Adress Location is    "<<arrinode[cinodenumber].pointer[i]*4096<<"    Block Number is "<<freedatablocknum<<endl;

            char buffer[4096];

            strcpy(buffer,data.c_str());

            fwrite(buffer,1,4096,diskpointer);

        }
        else
        {
            long long int add=fseek(diskpointer,arrinode[cinodenumber].pointer[i]*4096,SEEK_SET);

            char buffer[4096];

            strcpy(buffer,data.c_str());

            myfile<<"Adress Location is    "<<arrinode[cinodenumber].pointer[i]*4096<<"    Block Number is "<<arrinode[cinodenumber].pointer[i]<<endl;


            fwrite(buffer,1,4096,diskpointer);

        }

    }

   

    cout<<"Data Written Successfully"<<endl;
    

   

}






string convertToString(char* a)
{
    string s(a);
  
    
  
    return s;
}

int filereadfun(int fdescipt)
{
    if(checkopen(fdescipt)==-1)
        return -1;
    else
    {
        string op;
        if(checkreadmodeopen(fdescipt)==-1)
            return -1; 
        else
        {
            int cinodenumber=filedescripmap[fdescipt].first;

            for(int i=0;i<datablock;i++)
            {
                if(arrinode[cinodenumber].pointer[i]==-1)
                    break;
                fseek(diskpointer,arrinode[cinodenumber].pointer[i]*4096,SEEK_SET);

                myfile<<"Read:  Adress Location is    "<<arrinode[cinodenumber].pointer[i]*4096<<"    Block Number is "<<arrinode[cinodenumber].pointer[i]<<endl;

                
                char tempbuff[4096];

                myfile<<"Freadval ==   "<<fread(tempbuff,sizeof(tempbuff),1, diskpointer);

               

                op=op+convertToString(tempbuff);


            }

            cout<<op<<endl;

            


        }
    }
    return 0;
}









int filecreatefun(string fname)
{
    
    if (fileinodemap.find(fname) == fileinodemap.end())
    {
        writelog("File is not already Present in the Disk");

        if(!checkinodespace())
        {


            if (!checkdatablockespace())
            {
                int navailabledatanodecount = fdatablockvect.back();
                fdatablockvect.pop_back();
                writelog("Free DataBlock Fetched");


                int navailableinodecount = finodevect.back();
                finodevect.pop_back();
                writelog("Free INode fetched");
                


              
                arrinode[navailableinodecount].pointer[0] = navailabledatanodecount;
               

                writelog("INode has been given the Data Block");

                fileinodemap[fname] = navailableinodecount;
                inodefilemap[navailableinodecount] = fname;

                const char* fnamec=fname.c_str();

                arrinode[navailableinodecount].inode_num = navailableinodecount;
                strcpy(arrinode[navailableinodecount].file_name, fnamec);

                

                writelog("File Has been Successfully Created");

                cout << "File is Created Successfully" << endl;
                return 1;
            }
            else
            {
                cout << "Can't Create the File since no more Data Blocks are Available " << endl;
                writelog("File Cannot be Created Since No DataBlocks are available");
                return -1;
            }
        }
        else
        {
            cout <<"Can't Create the File since no more Nodes are Available " <<endl;
            writelog("File Cannot be Created Since No Inodes are available");
            return -1;
        }
    }
    else
    {
        cout << "Create File Error : File already present !!!" << endl;
        return -1;
    }

    



}




int check_if_already_opened(int cinode)
{
    for (int i = 0; i <= 31; i++)
    {
        writevalcount=1;
        if (filedescripmap.find(i) != filedescripmap.end() && filedescripmap[i].first == cinode && (filedescripmodemap[i] == 1 || filedescripmodemap[i] == 2))
        {
           
            writelog("File has been already opened previously");
            writevalcount=2;
            cout << "File has been already opened previously with file descriptor  " << i << endl;
            return -1;
        }
    }
    return 1;

}


void fmodeprint()
{
    cout<<"Enter 0 to open file in Read Mode"<<endl;
    cout<<"Enter 1 to open file in Write Mode"<<endl;
    cout<<"Enter 2 to open file in Append Mode"<<endl;
}




int open_file(string fname)
{
    int filemode=-1; 
    if (fileinodemap.find(fname) != fileinodemap.end())
    {
        if (freefilesdesvect.size() != 0)
        {
            fmodeprint();
            while(filemode<0 || filemode>2)
            {
                cin>>filemode;
                if(filemode<0 || filemode>2)
                {
                    cout<<"Please Enter a Valid Choice"<<endl;
                }
            }

            writelog("Mode of File taken as input Sucessfully");

            int cinode=fileinodemap[fname];

            if(filemode==1 || filemode==2)
            {
                if(check_if_already_opened(cinode)==-1)
                {
                    return -1;
                }
            }

            int filedescidx = freefilesdesvect.back();
            freefilesdesvect.pop_back();

            writelog("A free file Descriptor has been popped");

            filedescripmap[filedescidx].first = cinode;
            filedescripmap[filedescidx].second = 0;
            filedescripmodemap[filedescidx] = filemode;

            writelog("File Descriptor mapping has been Done");

            ofilecnt++;

            writelog("No of opened files count is increased");

            cout << "File " << fname << " is opened  with file descriptor  " << filedescidx<<"  Successfully" << endl;

            writelog("File opened Successfully");

            return filedescidx;



        }
        else
        {
            cout << "Can't open File since  File descriptor is not available " << endl;
            return -1;
        }
    }
    else
    {
        cout << "Can't open File since File can't be found" << endl;
        return -1;
    }
}












int unmount_disk()
{
    if (active)
    {
        for (int i =131071; i >= 1743; i--)
        {
            sb.lisfreedblock[i] = true;
        }

        writelog("Super Block Stored at the Beginning of Virtual Disk");

        for (unsigned int i = 0; i < fdatablockvect.size(); i++)
        {
            sb.lisfreedblock[fdatablockvect[i]] = false;
        }

        writelog("Free Data Block list added");

        fdatablockvect.clear();

        writelog("Free Data Block Vector Emptied");

        for (int i = 0; i < 78644; i++)
        {
            sb.lisfreeinode[i] = true;
        }


        freeinodevect();

        writelog("Free Inode are Set to False");



        
        
       
        fseek(diskpointer, 0, SEEK_SET);
        int len=sizeof(struct super_block);

        writelog("Dis Pointer set to start of Virtual Disk");

        char sb_buff[len];

        memset(sb_buff, 0, len);

        writelog("Buffer Initialised to 0");

        memcpy(sb_buff, &sb, sizeof(sb));

        writelog("Data Copied to Buffer Successfully");

       

        fwrite(&sb,sizeof(struct super_block), 1,diskpointer);

        writelog("Super Block details stored to buffer successfully");


        char inode_buff[sizeof(arrinode)];
        memset(inode_buff, 0, sizeof(arrinode));

        writelog("Buffer made Empty Successfully for INode Data");

        memcpy(inode_buff, arrinode, sizeof(arrinode));

        writelog("Inode Buff Data written to the Buffer Successfully");

      

        fwrite(arrinode, sizeof(struct inode),No_of_Inode,diskpointer);

        writelog("Inode Buff Data has been written successfully");

     

        clearallds();

        cout << "Disk Unmounted!!!" <<  endl;
        fclose(diskpointer);

        active = 0;

        
        return 0;



            

    }
    else
    {
        cout <<  "No Disk is Opened" << endl;
        return -1;
    }

}




void openfileslisprint()
{
    cout <<"List of opened files are:"<<endl;
    for (auto i : filedescripmap)
    {
        cout << inodefilemap[i.second.first] << " is opened with descriptor  " <<  i.first << "  in ";
        if (filedescripmodemap[i.first] == 0)
        {
            cout<< "Read mode"<<endl;
        }
        else if (filedescripmodemap[i.first] == 1)
        {
            cout<<"Write mode"<<endl;
        }
        else if (filedescripmodemap[i.first] == 2)
        {
            cout << "Append mode" <<endl;
        }
    }
    return;
}




void printfilelis()
{
    cout << "List of all files is" << endl;
    writelog("Listing all the files");
    for (auto i : fileinodemap)
    {
        cout << i.first << " with inode : " << i.second << endl;
    }
    writelog("List of all files is Printed");
    return;
}



void menuprint()
{
    cout << "Enter 1  to Create File" << endl;
    cout << "Enter 2  to Open file" << endl;
    cout << "Enter 3  to Read File" << endl;
    cout << "Enter 4  to write file" << endl;
    cout << "Enter 5  to append file" << endl;
    cout << "Enter 6  to close file" << endl;
    cout << "Enter 7  to delete file" << endl;
    cout << "Enter 8  to list of files" << endl;
    cout << "Enter 9  to list of opened files" << endl;
    cout << "Enter 10 to unmount" << endl;
    cout<<"------------------------------------------"<<endl;

    writelog("Menu Printed");
 
}




int close_file(int filedescript)
{
    if (filedescripmap.find(filedescript) == filedescripmap.end())
    {
        cout << "File Cannot be closed since it was not opened" <<  endl;
        writelog("File Cannot be closed since it was not opened");
        return -1;
    }
    else
    {
        filedescripmap.erase(filedescript);
        filedescripmodemap.erase(filedescript);

        writelog("File Closed");
    
        freefilesdesvect.push_back(filedescript);
        ofilecnt=ofilecnt-1;
        writelog("Count of opened files reduced");
        cout << "File closed successfully " <<  endl;

        writelog("File closed successfully");
        return 1;
    }
}





void deletefilefun(string fname)
{
    arrinode[fileinodemap[fname]].inode_num = -1;
    sb.lisfreeinode[fileinodemap[fname]]=false;
    fileinodemap[fname]=-1;

    cout<<"File Deleted Successfully"<<endl;
    
}


void diskoptions()
{
    int choice;
    while(true)
    {
        cin.ignore();
        cout<<"Enter 1 to Create Disk"<<endl;
        cout<<"Enter 2 to Mount Disk"<<endl;
        cout<<"Enter 3 to Exit"<<endl;
        cout<<"Enter your Choice    ";
        cin>>choice;
        cout<<"------------------------------------------"<<endl;
        if(choice==1)
        {
            cout<<"Enter the name of the Disk   ";
            string disk_name;
            cin>>disk_name;
            DiskCreate(disk_name);
            cout<<"------------------------------------------"<<endl;
        }
        else if(choice==2)
        {
            cout<<"Enter the name of the Disk   ";
            string disk_name;
            cin >> disk_name;
            if (mountDisk(disk_name))
            {
               
                int c;
                while(true)
                {
                    menuprint();
                    cout<<"Enter Your Choice   ";
                   
                    cin>>c;
                    if(c==1)
                    {
                        cout << "Enter Name of the File ";
                        string fnamestr;
                        cin >> fnamestr;
                        filecreatefun(fnamestr);
                        cout<<"------------------------------------------"<<endl;
                        
                    }
                    if(c==2)
                    {
                        cout << "Enter Name of the File to open  ";
                        string fnamestr;
                        cin >> fnamestr;
                        open_file(fnamestr);
                        cout<<"------------------------------------------"<<endl;
                        
                    }
                    if(c==3)
                    {
                        cout << "Enter filedescriptor to read  ";
                        int fd;
                        cin >> fd;
                        filereadfun(fd);
                        cin.clear();
                        cout.flush();
                        cout<<"------------------------------------------"<<endl;
                    }

                    if(c==4)
                    {
                        cout << "Enter filedescriptor to write  ";
                        int fd;
                        cin >> fd;
                        write_into_file(fd, 1);
                        cin.clear();
                        cout.flush();
                        cout<<"------------------------------------------"<<endl;
                       
                    }
                    
                    if(c==6)
                    {
                        cout << "Enter filedescriptor to close  ";
                        int filedest;
                        cin >> filedest;
                        close_file(filedest);
                        cout<<"------------------------------------------"<<endl;
                    }
                    if(c==7)
                    {
                        cout<<"Enter Name of the File to Delete   ";
                        string fnamestr;
                        cin>>fnamestr;
                        deletefilefun(fnamestr);
                        cout<<"------------------------------------------"<<endl;
                    }
                    if(c==8)
                    {
                        printfilelis();
                        cout<<"------------------------------------------"<<endl;
                    }
                    if(c==9)
                    {
                        openfileslisprint();
                        cout<<"------------------------------------------"<<endl;
                    }
                    if(c==10)
                    {
                        unmount_disk();
                        cout<<"------------------------------------------"<<endl;
                        break;
                    }
                }
            }
            
        }
        else if(choice==3)
        {
            cout<<"Good Bye!!"<<endl;
            exit(1);
        }
        else
        {
            cout<<"Enter a Valid Choice"<<endl;
        }
    }
}




int main()
{
    myfile.open("progLog.txt",ios::trunc);
    long long int v=(sizeof(struct super_block)+(sizeof(struct inode)*No_of_Inode))/block_size;
    diskoptions();
  
    writelog("Inside the main Function");
    return 0;
}