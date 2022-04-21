#include <ctime>
#include <cstring>
#include <fstream>
#include <vector>
#include "miracl.h"
#include "big.h"
#include "ecn.h"

using namespace std;
#define N  //文件数量
#define M 
#define KEYLEN 
#define DLEN 
#define ALEN 

Miracl precision(60, 16);

struct TAG
{
    ECn p1;
    Big p2;
    Big wit;
};

struct ED
{
    char d_[N][DLEN];
    struct X
    {
        Big C[N][M];
        ECn U[N][M];
        struct V
        {
            ECn V1[N];
            Big V2[N];
        } v;
    } x;
};

struct AK
{
    ECn AK1;
    ECn AK2;
};

class Scheme
{
public:
    // ParamGen
    Scheme();

    ED Setup(const char D[][DLEN], const char W[][M][KEYLEN]);

    AK Share(const bool *S);

    ECn TokenGen(const ECn &AK1, const char *w);

    void Test(const ED &ed, const bool *S, const ECn &T_u);

    bool Verify(const AK &ak, const char *w, const bool *S, const vector<int> &RS, const vector<TAG> &Tags);

private:
    miracl *mip;
    ECn g, pk, gn[N];
    Big q, sk;

    // fake function
    bool AccVerify() { return false; }
    Big GenWitness() { return rand(80, 2); }
    Big Accumulate(const char W[M][KEYLEN], Big n, Big u) { return rand(80, 2); }

    // util
    void Enc(char *buff, int len);
    Big H_(ECn p);

    static Big Hash(const char *m, int len);
    Big bigXor(Big a, Big b) { return lxor(a, b); }
};

//哈希函数H
Big Scheme::Hash(const char *m, int len)
{ /* compute hash function */
    char s[20];
    Big h;
    sha sh;
    shs_init(&sh);
    for (int i = 0; i < len; i++)
        shs_process(&sh, m[i]);
    shs_hash(&sh, s);
    h = from_binary(10, s); // 取哈希值前10位
    return h;
}

void Scheme::Enc(char *buff, int len)
{
    int i, nk = 16;
    aes a;
    char key[32];
    char iv[16];
    for (i = 0; i < 32; i++)
        key[i] = 0;
    key[0] = 1;
    for (i = 0; i < 16; i++)
        iv[i] = i;
    // printf("\nKey Size= %d bits\n", nk * 8);
    if (!aes_init(&a, MR_CBC, nk, key, iv))
    {
        printf("Failed to Initialize\n");
        exit(0);
    }
    aes_encrypt(&a, buff);
}

//哈希函数H_
Big Scheme::H_(ECn p)
{ /* compute hash function */
    Big x, y;
    char cx[200], cy[200];
    p.getxy(x, y);
    to_binary(x, 200, cx);
    to_binary(y, 200, cy);
    strcat(cx, cy);
    return Hash(cx, strlen(cx));
}

Scheme::Scheme()
{

    mip = &precision;

    // time_t seed;
    // time(&seed);
    // irand((long)seed); /* change parameter for different values */
    irand(1999l); // 置随机种子

    // 椭圆曲线参数读入
    Big a, b, p, x, y;
    int bits;
    ifstream common("common.ecs"); /* construct file I/O streams */
    /* get common data */
    common >> bits;
    mip->IOBASE = 16;
    common >> p >> a >> b >> q >> x >> y;
    mip->IOBASE = 10;

    // 系统参数生成
    ecurve(a, b, p, MR_BEST); // means use PROJECTIVE if possible, else AFFINE coordinates
    g = ECn(x, y);            //生成元
    for (int i = 0; i < N; i++)
    {
        a = rand(q);
        gn[i] = a * g;
    }
}

ED Scheme::Setup(const char D[][DLEN], const char W[][M][KEYLEN])
{
    sk = rand(160, 2);
    pk = sk * g;
    Big A[N];
    ED ed;
    for (int i = 0; i < N; i++)
    {
        Big t = rand(q);
        A[i] = Accumulate(W[i], rand(160, 2), rand(160, 2));
        for (int j = 0; j < M; j++)
        {
            ed.x.C[i][j] = t + sk * Hash(W[i][j], KEYLEN);
        }
        for (int k = 0; k < N; k++)
        {
            ed.x.U[i][k] = t * gn[k];
        }
        ed.x.v.V1[i] = t * g;
        ed.x.v.V2[i] = bigXor(A[i], H_((t + sk) * g));
        strcpy(ed.d_[i], D[i]);
    }
    return ed;
}

AK Scheme::Share(const bool *S)
{
    AK ak;
    ak.AK2 = pk;
    for (int i = 0; i < N; i++)
    {
        if (S[i])
            ak.AK1 += sk * gn[i];
    }
    return ak;
}

ECn Scheme::TokenGen(const ECn &AK1, const char *w)
{
    return Hash(w, KEYLEN) * AK1;
}

void Scheme::Test(const ED &ed, const bool *S, const ECn &T_u)
{
    for (int i = 0; i < N; i++)
    {
        ECn left = g, right = g;
        for (int k = 0; k < N; k++)
        {
            if (S[k])
            {
                right += ed.x.U[i][k];
                right += T_u;
            }
        }
        for (int j = 0; j < M; j++)
        {
            left = g;
            for (int k = 0; k < N; k++)
            {
                if (S[k])
                {
                    left += ed.x.C[i][j] * gn[k];
                }
            }
            if (left == right)
            {
                cout << "[*] Find it!"<<endl;
                cout << "[+] The document index is " << i << endl;
                break;
                /*
                TAG tag;
                RS.push_back(i);
                tag.p1 = ed.x.v.V1[i];
                tag.p2 = ed.x.v.V2[i];
                tag.wit = GenWitness();
                Tags.push_back(tag); 
                */
            }
        }
    }
}

bool Scheme::Verify(const AK &ak, const char *w, const bool *S, const vector<int> &RS, const vector<TAG> &Tags)
{
    Big A_[N];
    for (int i = 0; i < N; i++)
    {
        if (S[i])
        {
            ECn temp;
            temp += ak.AK2;
            temp += Tags[i].p1;
            A_[i] = bigXor(H_(temp), Tags[i].p2);
        }
    }
    return AccVerify();
}

int main()
{

    //文件
    const char D[N][DLEN] = { };

    //关键字集
    const char W[N][M][KEYLEN] = { };
    bool S[N] = {true};

    // a test keyword
    char w[] = { };
    Scheme scheme;
    ED ed = scheme.Setup(D, W);
    AK ak = scheme.Share(S);
    ECn T_u = scheme.TokenGen(ak.AK1, w);
    long start = clock();
    scheme.Test(ed, S, T_u);
    printf("[*] Test Running Time: %.6fms\n", (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0);
    return 0;
}


