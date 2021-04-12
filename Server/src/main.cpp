#include <TCPKernel.h>


int main(int argc,char *argv[])
{
    TcpKernel * pKernel = new TcpKernel;


    if(pKernel->Open() )
    {
        while(1)
        {
            printf("Server Running...\n");
            sleep(_DEF_TIMEOUT*3);
        }
    }
    pKernel->Close();

    return 0;
}
